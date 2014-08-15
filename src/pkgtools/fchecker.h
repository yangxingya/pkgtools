//!
/// brief: fholder.h define file opener
/// usage: you can use c-like 'FILE *fp' or win32-api
///        HANDLE *hd =...;

#if !defined(pkgtools_fchecker_h_)
#define pkgtools_fchecker_h_

#include <stdio.h>
#include <io.h>
#include <cclib/types.h>
#include <cclib/smartptr_def.h>
#include <cclib/win32/pathutil.h>
#include <cclib/win32/right.h>
#include <cclib/win32/autores.h>
#include "except.h"
#include "vsscopyer.h"

namespace file {

using namespace cclib;
using namespace err;

#pragma warning(push)
#pragma warning(disable: 4996)  /// disable fopen warning.

struct fholder
{
    //!
    /// brief: constructor
    /// argvs: name-> filename
    ///        max_effort-> if file open failed, try max effort???
    fholder(std::string const& name, bool max_effort = true)
        : name_(name)
        , max_effort_(max_effort)
        , fhd_(open(name))
        , len_(0L)
    {
        if (fhd_.get() != NULL) {
            len_ = len();
            DLOG(INFO) << "Open file: \"" << name_ << "\" successful!";
            return;
        }

        /// fp_ == NULL

        if (!max_effort_) {
            std::string error = "Open file: \"";
            error += name_;
            error += "\" no max effort, so exit";
            LOG(ERROR) << error;
            throw pkg_error(ERROR_OpenFileFailed, error);
        }

        /// max_effort_ == true;
        /// do 3rd test:
        ///     1. if name_ is a special path<system path>. remove parent dir trustedinstaller right.
        ///     2. copy file to volume root dir, then open
        ///     2. if 1st is failed, then do vss(volume shadow copy) then copy.
        /// if 1 and 2 all failed, then throw exception.

        /// 1. remove trustedinstaller right when name_ is a special path.
        if (win32::is_special(name_)) {
            std::string pdir = win32::pdir(name_);
            if (win32::rm_trustedinstaller(pdir)) {
                fhd_.reset(open(name_));
                if (fhd_.get() != NULL) {
                    len_ = len();
                    LOG(INFO) << "fholder: remove trustedinstaller and open: \"" << name_ << "\" successful!";
                    return;
                }
                LOG(WARNING) << "fholder: remove trustedinstaller but open failed: \"" << name_ << "\"";
            }
        }

        /// 2. copy to root dir and open.
        std::string tofile = copy_to_root();
        if (!tofile.empty()) {
            fhd_.reset(open(tofile, true));
            if (fhd_.get() != NULL) {
                len_ = len();
                LOG(INFO) << "fholder: copy to root and open: \"" << tofile << "\" successful!";
                return;
            }
            LOG(WARNING) << "fholder: copy to root but open failed: \"" << tofile << "\"";
        }

        /// 3. vss copy.
        vss::copyer &copyer = vss::makecopyer();
        tofile = copyer.copy(name_);
        if (!tofile.empty()) {
            fhd_.reset(open(tofile));
            if (fhd_.get() != NULL) {
                len_ = len();
                LOG(INFO) << "fholder: vss copy and open: \"" << tofile << "\" successful!";
                return;
            }
            LOG(WARNING) << "fholder: vss copy succeed but open failed: \"" << tofile << "\"";
        } 

        /// fp == NULL || tofile.empty() 

        std::string error = "Open file: \"";
        error += name_;
        error += "\" do max effort, so exit";
        LOG(ERROR) << error;
        throw pkg_error(ERROR_OpenFileFailed, error);
    }

    /// read file content to buf, and the max read len is
    /// rdlen or file real left length less rdlen.
    int read(void* buf, size_t rdlen)
    {
        if (fhd_.get() == NULL) return 0;

        ulong_t rrdlen = 0;
        ReadFile(fhd_.get(), buf, rdlen, &rrdlen, NULL);
        return rrdlen;
    }

    /// return file length, supported over 4G file.
    uint64_t length() const { return len_; }
private:
    std::string name_;
    bool max_effort_;
    uint64_t len_;

    win32::AutoHANDLE fhd_;


    uint64_t len() 
    {
        llong_t len = {0};
        GetFileSizeEx(fhd_.get(), (PLARGE_INTEGER)&len);
        return len.quad;
    }

    std::string copy_to_root()
    {
        std::string sname = win32::short_name(name_);
        std::string root = win32::root(name_);
        std::string rootname = root;

        win32::add_sep(rootname);
        rootname += sname;

        if (!win32::copy(rootname, name_))
            return "";

        return rootname;
    }

    HANDLE open(std::string const& file, bool del_when_close = false) 
    {
        DWORD attr = FILE_FLAG_SEQUENTIAL_SCAN;
        if (del_when_close) 
            attr |= FILE_FLAG_DELETE_ON_CLOSE;
        else
            attr |= FILE_ATTRIBUTE_NORMAL;
        HANDLE hd = CreateFileA(
                file.c_str(),
                GENERIC_READ,
                FILE_SHARE_READ,
                NULL,
                OPEN_EXISTING,
                attr,
                NULL
                );
        if (hd == INVALID_HANDLE_VALUE) {
            hd = NULL;
            LOG(WARNING) << "fholder: open error, code:" << GetLastError() << ", file: \"" << file << "\" failed";
        }
        return hd;
    }
};

#pragma warning(pop) 

const std::string kprogramdata = win32::sysroot() + win32::kseparator + "ProgramData";

struct fchecker
{
    shared_ptr<fholder> check(std::string const& name, bool effort = true)
    {
        if (filter(name))
            return shared_ptr<fholder>();

        std::string name_lower = to_lower(name);
        std::map<std::string, shared_ptr<fholder>>::iterator it;
        if ((it = name_to_holder_.find(name_lower)) != name_to_holder_.end())
            return it->second;

        /// not find.
        shared_ptr<fholder> opener;
        try {
            opener.reset(new fholder(name, effort));
        } catch (except_base &ex) {
            LOG(ERROR) << "Open file failed! file:\"" << name << "\", error: " << ex.error();
            return shared_ptr<fholder>();
        }

        /// add to map.
        name_to_holder_[name_lower] = opener;
        return opener;
    }
    
    std::map<std::string, shared_ptr<fholder>>& map() { return name_to_holder_; }

private:
    std::map<std::string, shared_ptr<fholder>> name_to_holder_;

    bool filter(std::string const &path)
    {
        /// must filter "C:\ProgramData\xxx" path.
        if (start_with(path, kprogramdata, false))
            return true;

        return false;
    }
};

} // namespace file

#endif // pkgtools_fchecker_h_