//!
/// brief: fopener.h define file opener
/// usage: you can use c-like 'FILE *fp' or win32-api
///        HANDLE *hd =...;

#if !defined(pkgtools_fopener_h_)
#define pkgtools_fopener_h_

#include <stdio.h>
#include <io.h>
#include <cclib/types.h>
#include <cclib/win32/pathutil.h>
#include <cclib/win32/right.h>
#include "error.h"
#include "except.h"

namespace file {

using namespace cclib;

#pragma warning(push)
#pragma warning(disable: 4996)  /// disable fopen warning.

struct fopener
{
    //!
    /// brief: constructor
    /// argvs: name-> filename
    ///        max_effort-> if file open failed, try max effort???
    fopener(std::string const& name, bool max_effort = true)
        : name_(name)
        , max_effort_(max_effort)
        , fp_(fopen(name.c_str(), "rb"))
        , len_(0L)
        , tmp_deleted_(false)
    {
        if (fp_ != NULL) {
            getlen();
            DLOG(INFO) << "Open file: \"" << name_ << "\" successful!";
            return;
        }

        /// fp_ == NULL

        if (!max_effort_) {
            std::string error = "Open file: \"";
            error += name_;
            error += "\" no max effort, so exit";
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
                FILE *fp = fopen(name_.c_str(), "rb");
                if (fp != NULL) {
                    reset(fp);
                    getlen();
                    LOG(INFO) << "fopener: remove trustedinstaller and open: \"" << name_ << "\" successful!";
                    return;
                }
                LOG(WARNING) << "fopener: remove trustedinstaller but open failed: \"" << name_ << "\"";
            }
        }

        /// 2. copy to root dir and open.
        tmp_ = copy_to_root();
        if (!tmp_.empty()) {
            FILE *fp = fopen(tmp_.c_str(), "rb");
            if (fp != NULL) {
                reset(fp);
                tmp_deleted_ = true;
                getlen();
                LOG(INFO) << "fopener: copy to root and open: \"" << tmp_ << "\" successful!";
                return;
            }
            LOG(WARNING) << "fopener: copy to root but open failed: \"" << tmp_ << "\"";
        }

        /// 3. vss copy.


    }

    ~fopener() { reset(); }

    /// read file content to buf, and the max read len is
    /// rdlen or file real left length less rdlen.
    int read(void* buf, size_t rdlen)
    {
        if (fp_ == NULL) return 0;

        return fread(buf, 1, rdlen, fp_);
    }

    /// return file length, supported over 4G file.
    uint64_t length() const { return len_; }
private:
    std::string name_;
    bool max_effort_;
    FILE *fp_;
    uint64_t len_;

    bool tmp_deleted_;
    std::string tmp_;

    void getlen() 
    {
        if (fp_ == NULL) {
            len_ = 0;
            return;
        }

        /// if _filelengthi64 failed then return value is -1L,
        /// details:
        ///     http://msdn.microsoft.com/en-us/library/dfbc2kec.aspx
        /// then need changed to 0, because -1L cast to uint64 will 
        /// be max uint64 value.
        int64_t len = _filelengthi64(fp_->_file);
        len_ = (len == -1L) ? 0 : len;
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

    void reset(FILE *fp = NULL)
    {
        if (fp_) { 
            fflush(fp_); 
            fclose(fp_); 

            /// deleted when closed.
            if (tmp_deleted_) {
                win32::rmfile(tmp_);
                tmp_deleted_ = false;
            }
        } 

        fp_ = fp;
    }
};

#pragma warning(pop)  

} // namespace file

#endif // pkgtools_fopener_h_