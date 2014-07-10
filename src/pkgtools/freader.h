//!
/// freader.h define file reader
///

#if !defined(pkgtools_file_reader_h_)
#define pkgtools_file_reader_h_

#include <io.h>
#include <stdio.h>
#include <cclib/types.h>

struct FileReader
    FileReader(std::string const& file, std::string const& mode)
        : fp_(fopen(file.c_str(), mode.c_str()))
        , good_(fp_ != 0)
    {
    }

    ~FileReader() { if (fp_) fclose(fp_); }
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

#endif // pkgtools_file_reader_h_