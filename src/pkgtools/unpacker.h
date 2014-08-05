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
            reader_.read(&buf_[0], buf_.size());
                
            if (header_->compress == pkg::kcompno) {
                writer->write(&buf_[0], buf_.size());
                continue;
            }
            /// compress need decompress.

        }
        reader_.read(&buf_[0], leave);
        if (header_->compress == pkg::kcompno)
            writer->write(&buf_[0], leave);
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

};

} // namespace pkg

#endif // pkgtools_unpacker_h_