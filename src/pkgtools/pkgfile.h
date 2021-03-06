//!
/// brief: pkgfile.h define package file operate.
///

#if !defined(pkgtools_pkg_file_h_)
#define pkgtools_pkg_file_h_

#include <string>
#include <sstream>
#include <glog/logging.h>
#include <cclib/types.h>
#include <cclib/win32/pathutil.h>
#include <cclib/win32/right.h>
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
        : writer_(new fwrapper(file, "wb+")) 
    {
        /// some time, you write file to system will not right, you must delete parent dir 
        /// special right, for example: trustedinstaller right.
        if (!writer_->good()) {
            if (win32::is_special(file)) {
                std::string pdir = win32::pdir(file);
                if (win32::rm_trustedinstaller(pdir)) {
                    writer_.reset(new fwrapper(file, "wb+")); 
                    LOG(INFO) << "fwrapper: remove parent dir trustedinstaller : \"" << pdir << "\" successful!";    
                }
            }
        }

        std::string error = "Writer File: ";
        error += file;
        error += (writer_->good() ? " create successful!" : " create failed!");
        if (!writer_->good()) {
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
        uint8_t *ptr = (uint8_t *)data;

        for (uint32_t i = 0; i < times; ++i) {
            if (!writer_->write(ptr, kuint32max))
                return false;
            ptr += kuint32max;
        }
        return writer_->write(ptr, leave);
    }

    bool write(void const* buf, size_t len)
    {
        return writer_->write(buf, len);
    }

    bool seek(int64_t pos) { return writer_->seek(pos); }
private:
    shared_ptr<fwrapper> writer_;
};

struct Reader
{
    Reader(std::string const& file)
        : reader_(file, "rb")
    {
        check();

        std::string error = "Reader File: ";
        error += file;
        error += (reader_.good() ? " open successfule!" : " open failed!");
        if (!reader_.good()) {
            LOG(ERROR) << error;
            throw pkg_error(ERROR_OpenFileFailed, error.c_str());
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