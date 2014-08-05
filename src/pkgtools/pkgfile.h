//!
/// brief: pkgfile.h define package file operate.
///

#if !defined(pkgtools_pkg_file_h_)
#define pkgtools_pkg_file_h_

#include <string>
#include <sstream>
#include <glog/logging.h>
#include <cclib/types.h>
#include "fwrapper.h"
#include "fwrapper.h"
#include "except.h"
#include "pkgdef.h"

namespace pkg {

using namespace cclib;
using namespace err;
using ::file::fwrapper;

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

    bool seek(int64_t pos) { return writer_.seek(pos); }
private:
    fwrapper writer_;
};

struct Reader
{
    Reader(std::string const& file)
        : reader_(file, "rb")
    {
        check();

        std::string error = "Open Package File: ";
        error += file;
        error += (reader_.good() ? " successfule!" : " failed!");
        if (!reader_.good()) {
            LOG(ERROR) << error;
            throw pkg_error(ERROR_CreatePkgFileFailed, error.c_str());
        }
        DLOG(INFO) << error;
    }

    bool read(void *buf, size_t buflen) { return reader_.read(buf, buflen); }
    bool seek(int64_t pos) { return reader_.seek(pos); }
   
private:
    fwrapper reader_;
    void check() 
    {
        /// only check header magic and pkglen == filesize
        header_t hdr;
        if (!read(&hdr, sizeof(header_t))) {
            std::string error = "Package read package header failed!";
            LOG(ERROR) << error;
            throw pkg_error(ERROR_PkgHdrError, error);
        }

        if (hdr.magic != *(uint32_t *)kmagic) {
            std::string error = "Package read package header magic failed!";
            LOG(ERROR) << error;
            throw pkg_error(ERROR_PkgHdrError, error);
        }

        uint64_t flen = (uint64_t)reader_.length();
        if (hdr.pkglen != flen) {
            std::stringstream serr;
            serr << "Package read package header pkglen failed! file size: "
                 << reader_.length() << ", header pkglen: "
                 << hdr.pkglen;
            std::string error;
            serr >> error;
            LOG(ERROR) << error;
            throw pkg_error(ERROR_PkgHdrError, error);
        }

        ///TODO:: crc32 not implement, so return true ever.

        /// need seek file pointer to start.
        seek(0);
    }
};

} // namespace pkg

#endif // pkgtools_pkg_file_h_