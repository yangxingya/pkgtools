//!
/// brief: unpacker.h define unpack installed-packate api
/// usage: extractor/installer/uninstaller will use unpacker
/// detail: 
///         +---------+                 +--------+
///         |extractor|   ---------->   |unpacker|
///         +---------+                 +--------+
///
///         +---------+                 +--------+
///         |installer|   ---------->   |unpacker|
///         +---------+                 +--------+
///
///         +-----------+               +--------+
///         |uninstaller| ---------->   |unpacker|
///         +-----------+               +--------+
///


#if !defined(pkgtools_unpacker_h_)
#define pkgtools_unpacker_h_

#include <vector>
#include <string>
#include <cclib/types.h>
#include <cclib/algo/crc.h>
#include <cclib/win32/pathutil.h>
#include "pkgdef.h"
#include "pkgfile.h"
#include "fwrapper.h"

namespace pkg {

using namespace cclib;

struct unpacker
{
    unpacker(std::string const& file)
        : reader_(file)
    {
        crc32_valid(file);
        ex_header();
        buf_.resize(k1mega * 10);
    }

    header_t header() const { return *header_; }
    const std::vector<entry_t>& entrys() const { return entrys_; }
    const std::vector<std::string>& arglist() const { return args_; }
    const std::vector<dinfo_t>& dinfos() const { return dinfos_; }

    //!
    /// brief: tofile is extract file from package.
    ///        if dest existed will throw exception,
    ///        then up level(caller) will catch the
    ///        exception and deal it.
    uint64_t tofile(std::string const& to, uint64_t dindex)
    {
        std::string ex_to = to;
        std::string ex_to_pdir = win32::pdir(ex_to);
        if (!win32::existed(ex_to_pdir))
            win32::mkdirtree(ex_to_pdir);
        
        uint64_t reallen = dinfomgr_->infos[dindex].reallen;
        uint64_t complen = dinfomgr_->infos[dindex].cmplen;

        int64_t start = (int64_t)(header_->data + dinfomgr_->infos[dindex].start);
        reader_.seek(start);

        uint64_t times = reallen / buf_.size();
        uint32_t leave = (uint32_t)(reallen % buf_.size());

        shared_ptr<Writer> writer;
        try {
            writer.reset(new Writer(ex_to));
        } catch (except_base) {
            return 0;
        }

        for (uint64_t i = 0; i < times; ++i) {
            if (!reader_.read(&buf_[0], buf_.size()))
                return 0L;
                
            if (header_->compress == pkg::kcompno) {
                if (!writer->write(&buf_[0], buf_.size()))
                    return 0L;
                continue;
            }
            /// compress need decompress.

        }
        if (!reader_.read(&buf_[0], leave))
            return 0L;

        if (header_->compress == pkg::kcompno)
            if (!writer->write(&buf_[0], leave))
                return 0L;
        /// compress need decompress.
        /// else
        return reallen;
    }
private:
    Reader reader_;

    std::vector<uint8_t> pkghdr_;
    header_t   *header_;
    entrymgr_t *entrymgr_;
    argvmgr_t  *argvmgr_;
    dinfomgr_t *dinfomgr_;

    std::vector<entry_t> entrys_;
    std::vector<std::string> args_;
    std::vector<dinfo_t> dinfos_;

    std::vector<uint8_t> buf_;

#pragma region extract
    bool ex_header()
    {
        header_t hdr;
        reader_.read(&hdr, sizeof(header_t));
        pkghdr_.resize((uint32_t)hdr.data);

        reader_.seek(0);
    
        reader_.read(&pkghdr_[0], pkghdr_.size());
        header_   = (header_t   *)&pkghdr_[0];
        entrymgr_ = (entrymgr_t *)&pkghdr_[(uint32_t)header_->entrymgr];
        argvmgr_  = (argvmgr_t  *)&pkghdr_[(uint32_t)header_->argvmgr ];
        dinfomgr_ = (dinfomgr_t *)&pkghdr_[(uint32_t)header_->dinfomgr];

        ex_argv();

        entrys_.assign(entrymgr_->entrys, entrymgr_->entrys + entrymgr_->count);
        dinfos_.assign(dinfomgr_->infos, dinfomgr_->infos + dinfomgr_->count);
        
        return true;
    }

    void ex_argv()
    {
        std::string tmp;
        char *ptr = argvmgr_->start;

        for (uint64_t i = 0L; i < argvmgr_->count; ++i) {
            tmp = ptr;
            args_.push_back(tmp);

            ptr += tmp.length() + 1;
            DLOG(INFO) << "Extract args " << i << ": " << tmp;
        }   
    }
#pragma endregion extract all parts.

#pragma region crc32
    void crc32_valid(std::string const& file)
    {
        reader_.seek(0);
        header_t hdr;
        reader_.read(&hdr, sizeof(header_t));
        reader_.seek(0);

        algo::crc32 calc_crc(file, sizeof(hdr.magic) + sizeof(hdr.crc32));
        if (!calc_crc.ready()) {
            std::string error = "Unpacker: crc open file failed! file: \"";
            error += file;
            error += "\"";
            LOG(ERROR) << error;
            throw pkg_error(ERROR_OpenFileFailed, error);
        }

        if (hdr.crc32 != calc_crc.value()) {
            std::stringstream ss;
            ss  << "Unpacker: crc valid failed, read: 0x" 
                << std::hex << hdr.crc32
                << ". calc: 0x" << calc_crc.value()
                << ". file: \"" << file << "\"";
            std::string error;
            ss >> error;

            LOG(ERROR) << error;
            throw pkg_error(ERROR_Crc32CheckFailed, error);
        }

        LOG(INFO) << "Unpacker: check crc32 successful";
    }

#pragma endregion crc32 valid.

};

} // namespace pkg

#endif // pkgtools_unpacker_h_