//!
/// brief: pkgfile.h define package file operate.
///

#if !defined(pkgtools_pkg_file_h_)
#define pkgtools_pkg_file_h_

#include <string>
#include <glog/logging.h>
#include <cclib/types.h>
#include "fwriter.h"
#include "freader.h"
#include "error.h"
#include "except.h"
#include "pkgdef.h"

namespace pkg {

using namespace cclib;
using ::file::fwriter;
using ::file::freader;

struct Writer
{
    Writer(std::string const& file)
        : writer_(file, "wb+") 
    {
        std::string error = "Create Package File: ";
        error += file;
        error += (writer_.good() ? " successfule!" : " failed!");
        if (!writer_.good()) {
            LOG(ERROR) << error;
            throw pkg_error(ERROR_CreatePkgFileFailed, error.c_str());
        }
        DLOG(INFO) << error;
    }

    template<typename T>
    bool write(T const* data)
    {
        uint64_t len = length(data);
        uint32_t times = (uint32_t)(len >> kuint64high32shift);
        uint32_t leave = (uint32_t)(len & kuint32max);
        uint64_t writed = 0;
        uint8_t *ptr = (uint8_t *)data;
        for (uint32_t i = 0; i < times; ++i) {
            writed += writer_.write(ptr, kuint32max);
            ptr += kuint32max;
        }
        writed += writer_.write(ptr, leave);
        return writed == len; 
    }

    bool write(void const* buf, size_t len)
    {
        return writer_.write(buf, len);
    }
private:
    fwriter writer_;
};

struct Reader
{
    Reader(std::string const& file)
        : reader_(file, "rb") 
    {
        std::string error = "Create Package File: ";
        error += file;
        error += (reader_.good() ? " successfule!" : " failed!");
        if (!reader_.good()) {
            LOG(ERROR) << error;
            throw pkg_error(ERROR_CreatePkgFileFailed, error.c_str());
        }
        DLOG(INFO) << error;
    }

    bool read(void *buf, size_t len)
    {
        return reader_.read(buf, len);
    }
private:
    freader reader_;
};

} // namespace pkg

#endif // pkgtools_pkg_file_h_