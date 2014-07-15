//!
/// brief: fopener.h define file opener
/// usage: you can use c-like 'FILE *fp' or win32-api
///        HANDLE *hd =...;

#if !defined(pkgtools_fopener_h_)
#define pkgtools_fopener_h_

#include <stdio.h>
#include <io.h>
#include <cclib/types.h>
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
        /// do 2nd test:
        ///     1. copy file to volume root dir, then open
        ///     2. if 1st is failed, then do vss(volume shadow copy) then copy.
        /// if 1 and 2 all failed, then throw exception.


        ///TODO:: 1. copy file to volume root dir and open.
        /// getlen();
        ///TODO:: 2. vss copy file to vss volume and open.
        /// getlen();

    }

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
};

#pragma warning(pop)  

} // namespace file

#endif // pkgtools_fopener_h_