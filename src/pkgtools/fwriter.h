//!
/// fwriter.h define file writer.
///

#if !defined(pkgtools_file_writer_h_)
#define pkgtools_file_writer_h_

#include <io.h>
#include <stdio.h>
#include <cclib/types.h>

namespace file {

using namespace cclib;

#pragma warning(push)
#pragma warning(disable: 4996)  /// disable fopen warning.

struct fwriter
{
    fwriter(std::string const& file, std::string const& mode)
        : fp_(fopen(file.c_str(), mode.c_str()))
        , good_(fp_ != 0)
    {
    }

    ~fwriter() { if (fp_) fclose(fp_); }
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

    int64_t length() 
    {
        if (fp_ == 0) return 0;
        return _filelengthi64(fp_->_file);
    }

private:
    FILE *fp_;
    bool good_;
};

#pragma warning(pop)  

} // namespace file

#endif // pkgtools_file_writer_h_