//!
/// brief: pkgfile.h define package file operate.
///

#if !defined(pkgtools_pkg_file_h_)
#define pkgtools_pkg_file_h_

#include <string>
#include "filecreator.h"
#include <glog/logging.h>
#include "error.h"
#include "except.h"
#include "pkgdef.h"

const uint32_t kuint32max = 0xffffffff;
const uint32_t kuint64high32shift = 32;

namespace pkg {

struct Writer
{
    Writer(std::string const& file)
        : creator_(file, "wb+") 
    {
        std::string error = "Create Package File: ";
        error += file;
        error += (creator_.good() ? " successfule!" : " failed!");
        if (!creator_.good()) {
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
            writed += creator_.write(ptr, kuint32max);
            ptr += kuint32max;
        }
        writed += creator_.write(ptr, leave);
        return writed == len; 
    }
    
private:
    FileCreator creator_;
};

struct Reader
{
    Reader(std::string const& file) {}
};

} // namespace pkg

#endif // pkgtools_pkg_file_h_