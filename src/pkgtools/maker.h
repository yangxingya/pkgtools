//!
/// brief: pkgmaker.h define pkg maker 
///

#if !defined(pkgtools_pkg_maker_h_)
#define pkgtools_pkg_maker_h_

#include <cclib/types.h>
#include <cclib/smartptr_def.h>
#include "error.h"
#include "except.h"
#include "argv.h"
#include "entrydef.h"
#include "pkgdef.h"
#include "argv.h"
#include "pkgfile.h"
#include "fappender.h"

namespace pkg {

using namespace cclib;
using ::file::fopener;
using ::file::fappender;

const std::string kdefaultoutput = "./pkg.dat";

struct Maker
{
    Maker(argv::AutoArgvList const& arglist, std::string const& to = kdefaultoutput)
        : arglist_(arglist), to_(to) {}

    int make() 
    {
        try {
            extractArgv();
        } catch (except_base &ex) {
            return ex.error();
        }

        
        header_t test;
        uint64_t len = make_header(&test);
        ///HACK if len > int_max ???
        /// need improve...

        std::vector<uint8_t> buff;
        buff.resize((uint32_t)len);

        header_t   *header   = (header_t   *)&buff[0];
        entrymgr_t *entrymgr = (entrymgr_t *)&buff[(uint32_t)test.entrymgr];
        argvmgr_t  *argvmgr  = (argvmgr_t  *)&buff[(uint32_t)test.argvmgr ];
        dinfomgr_t *dinfomgr = (dinfomgr_t *)&buff[(uint32_t)test.dinfomgr];

        make_header(header  );
        make_entry (entrymgr);
        make_argv  (argvmgr );
        make_dinfo (dinfomgr);

        shared_ptr<Writer> writer;
        try {
            writer.reset(new Writer(to_));
        } catch (except_base &ex) {
            return ex.error();
        }

        /// write pkg all header.
        writer->write(&buff[0], buff.size());

        /// data appender 
        /// file data can compress or not. depends you policy.
        std::vector<uint64_t> lenlist; 
        fappender appender(writer.get(), lenlist);
        std::for_each(pkg_files_.begin(), pkg_files_.end(), appender);

        /// modify dinfomgr_t dinfos array. comp len.
        /// 
        uint64_t datalen = modify(dinfomgr, lenlist);
        modify(header, datalen);

        /// modify to file.
        writer->seek((int64_t)header->dinfomgr);
        writer->write(dinfomgr);

        writer->seek(0L);
        writer->write(header);

        return ERROR_Success;
    }

private:
    argv::AutoArgvList arglist_;
    std::string to_;

    std::vector<std::string>  pkg_argvs_;
    std::vector<entry_t> pkg_entrys_;
    std::vector<shared_ptr<fopener> > pkg_files_;
    uint64_t pkg_argvs_len_;

    void extractArgv()
    {
        // extract pkg argv string.
        std::map<uint64_t, uint64_t> args_map;
        entry::transfer argvtransfer(pkg_argvs_, args_map);
        std::for_each(arglist_.begin(), arglist_.end(), argvtransfer);

        /*
        if (pkg_argvs_.size() != arglist_.size()) {
            std::string error = "After transfer argv, the transfer size is not equal original argv list size";
            LOG(ERROR) << error;
            throw pkg_error(ERROR_TransferArgvError, error); 
        }
        */

        for (size_t i = 0; i < arglist_.size(); ++i)
            DLOG(INFO) << "    args: " << pkg_argvs_[i];

        pkg_argvs_len_ = 0L;
        entry_t entry;

        for (size_t i = 0; i < arglist_.size(); ++i) {
            entry = argv::helper::extract(arglist_[i]);
            entry.strindex = args_map[i];

            entry.dtaindex = kinvalid;
            /// if argv is file, file data index need calc.
            if (arglist_[i]->type() == entry::kFile) {
                argv::FileArgv *fargv = (argv::FileArgv *)arglist_[i].get();
                pkg_files_.push_back(fargv->get());
                entry.dtaindex = pkg_files_.size() - 1;
            }
            
            pkg_entrys_.push_back(entry);
            pkg_argvs_len_ += pkg_argvs_[i].length() + 1;
        }
    }

    
#pragma region make

    uint64_t make_header(header_t *header)
    {
        header->magic = *(uint32_t *)(kmagic);
        header->crc32 = kinvalidcrc32;
        header->version  = kcurrentver;
        header->compress = kcompno;

        header->entrymgr = sizeof(header_t);
        header->argvmgr  = header->entrymgr +
            sizeof(entrymgr_t) + (pkg_entrys_.size() - 1) * sizeof(entry_t);
        header->dinfomgr = header->argvmgr + 
            sizeof(argvmgr_t) + pkg_argvs_len_ - 1;
        header->data = header->dinfomgr + 
            sizeof(dinfomgr_t) + (pkg_files_.size() - 1) * sizeof(dinfo_t);

        ///TODO::header.pkglen = header.data + datalen;
        header->pkglen = 0L;

        return header->data;
    }

    void make_entry(entrymgr_t *entrymgr)
    {
        entrymgr->count = pkg_entrys_.size();
        memcpy(entrymgr->entrys, &pkg_entrys_[0], pkg_entrys_.size() * sizeof(entry_t));
    }

#pragma warning(push)
#pragma warning(disable: 4996)  /// disable strcpy warning.

    void make_argv(argvmgr_t *argvmgr)
    {
        argvmgr->count = pkg_argvs_.size();
        argvmgr->length = pkg_argvs_len_;
        char *pos = argvmgr->start;
        for (size_t i = 0; i < pkg_argvs_.size(); ++i) {
            /// strcpy will copy null-terminal.
            strcpy(pos, pkg_argvs_[i].c_str());
            pos += pkg_argvs_[i].length() + 1;
        }
    }

#pragma warning(pop)

    void make_dinfo(dinfomgr_t *dinfomgr)
    {
        dinfomgr->count = pkg_files_.size();

        /// set real length.
        dinfo_t *dptr = dinfomgr->infos;
        for (size_t i = 0; i < pkg_files_.size(); ++i) {
            dptr->reallen = pkg_files_[i]->length();
            dptr++;
        }
    }

#pragma endregion make header

#pragma region modify
    uint64_t modify(dinfomgr_t *dinfomgr, std::vector<uint64_t> const& clenlist)
    {
        
        dinfo_t *dptr = dinfomgr->infos;
        uint64_t offset = 0;

        /// modify dinfo_t cmplen and start offset.
        for (size_t i = 0; i < clenlist.size(); ++i) {
            dptr->cmplen = clenlist[i];
            dptr->start = offset;

            offset += clenlist[i];
            dptr++;
        }

        return offset;
    }

    void modify(header_t *header, uint64_t datalen)
    {
        header->pkglen = header->data + datalen;
    }
#pragma endregion modify part.

};

} // namespace pkg


#endif // pkgtools_pkg_maker_h_