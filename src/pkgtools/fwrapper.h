//!
/// fwrapper.h define c file api wrapper, support more than 4G file.
///

#if !defined(pkgtools_file_wrapper_h_)
#define pkgtools_file_wrapper_h_

#include <io.h>
#include <stdio.h>
#include <cclib/types.h>
#include <cclib/win32/pathutil.h>

namespace file {

using namespace cclib;

#pragma warning(push)
#pragma warning(disable: 4996)  /// disable fopen warning.

struct fwrapper
{
    fwrapper(
        std::string const& file, std::string const& mode
        , bool closed_with_deleted = false)
            : file_(file)
            , fp_(fopen(file.c_str(), mode.c_str()))
            , good_(fp_ != 0)
            , closed_with_deleted_(closed_with_deleted)
    {
    }

    ~fwrapper() 
    { 
        if (fp_) { 
            fflush(fp_); 
            fclose(fp_); 

            /// deleted when closed.
            if (closed_with_deleted_)
                win32::rmfile(file_);
        } 
    }
    bool good() const { return good_; }
    
    bool seek(int64_t pos) 
    { 
        if (fp_ == 0) return false;
        return _fseeki64(fp_, pos, SEEK_SET) == 0; 
    }

    bool tell(int64_t *pos) 
    {
        if (fp_ == 0) return false;
        *pos = _ftelli64(fp_);
        return true;
    }

    bool write(void const* buf, size_t buflen)
    {
        if (fp_ == 0) return false;
        return fwrite(buf, 1, buflen, fp_) == buflen;
    }

    bool read(void *buf, size_t buflen)
    {
        if (fp_ == 0) return false;
        return fread(buf, 1, buflen, fp_) == buflen;
    }

    int64_t length() 
    {
        if (fp_ == 0) return 0;
        return _filelengthi64(fp_->_file);
    }
private:
    std::string file_;
    FILE *fp_;
    bool good_;
    bool closed_with_deleted_;
};

#pragma warning(pop)  

} // namespace file

#endif // pkgtools_file_wrapper_h_