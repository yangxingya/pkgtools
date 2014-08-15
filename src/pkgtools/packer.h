//!
/// brief: packer.h define pack installed package api.
///

#if !defined(pkgtools_packer_h_)
#define pkgtools_packer_h_

#include <string>
#include <cclib/types.h>
#include <cclib/smartptr_def.h>
#include <cclib/algo/crc.h>

#include "except.h"
#include "fchecker.h"
#include "fappender.h"
#include "counter.h"
#include "argvdef.h"
#include "argv.h"
#include "pkgdef.h"
#include "pkgfile.h"


namespace pkg {

using namespace cclib;
using namespace argv;
using namespace file;

struct packer 
{
    packer(AutoArgvList const& arglist)
        : arglist_(arglist)
    {
        out();
    }

    int pack()
    {
        LOG(INFO) << "Pkg: -------------------> start";

        int ret;
        if ((ret = prepack()) != ERROR_Success) {
            LOG(ERROR) << "Pkg: pre-pack failed";
            return ret;
        }

        LOG(INFO) << "Pkg: pre pack";

        rm_bad_args();

        LOG(INFO) << "Pkg: remove bad args.";

        try {
            ready();
        } catch (except_base &ex) {
            LOG(ERROR) << "Pkg: XXXXXX ready error: " << ex.what();
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
            writer.reset(new Writer(out_));
        } catch (except_base &ex) {
            LOG(ERROR) << "Pkg: XXXXXX writer error: " << ex.what();
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

        /// close file.
        writer.reset();

        /// calc crc32;
        algo::crc32 calc_crc(out_, sizeof(header->magic) + sizeof(header->crc32));

        if (!calc_crc.ready()) {
            LOG(ERROR) << "Pkg: calc crc32 failed";
            return ERROR_CalcCrc32Failed;
        }

        /// modify crc32 section.
        header->crc32 = calc_crc.value();

        shared_ptr<fwrapper> modifyheader(new fwrapper(out_, "rb+"));
        modifyheader->seek(0L);
        modifyheader->write(header, sizeof(header_t));

        LOG(INFO) << "Pkg: -------------------> over";

        return ERROR_Success;
    }

private:
    AutoArgvList arglist_;
    std::string out_;

    std::vector<std::string> pkg_args_;
    std::vector<entry_t> pkg_entrys_;
    std::vector<shared_ptr<fholder>> pkg_files_;
    uint64_t pkg_args_len_;

    fchecker fchker_;
    fcounter fcnter_;
    acounter acnter_;

    void out()
    {
        AutoArgvList::iterator it;
        for (it = arglist_.begin(); it != arglist_.end(); ++it) {
            if ((*it)->type() == kOut) {
                /// find, get out and return.
                out_ = ((OutArgv*)((*it).get()))->dst();
                ///arglist_.erase(it);
                return;
            }
        }

        /// not find
        throw script_error(ERROR_ScriptNoOutArgs, "Script no out args line");
    }

#pragma region ready
    //!
    /// brief: pre-pack.
    int prepack()
    {
        /// file open checker.
        /// if file open check failed, then return failed.
        int ret;

        ArgvBase *arg;
        for (size_t i = 0; i < arglist_.size(); ++i) {
            if (arglist_[i]->pkgtFlags() & kpkgtmask) {
                arg = arglist_[i].get();
                switch (arglist_[i]->type()) {
                case kSetting:
                    {
                        SettingArgv *sargv = (SettingArgv*)arg;
                        if (sargv->pkgtFlags()) {
                            pkg::Setting setting(sargv->flags());
                            if ((ret = setting.doSet()) != ERROR_Success)
                                return ret;
                        }
                    }
                    break;
                case kAddf:
                    {
                        AddfArgv *fargv = (AddfArgv*)arg;
                        bool max_effort = !!(fargv->pkgtFlags() & kerrtry);
                        bool ignore_err = !!(fargv->pkgtFlags() & kerrignore);
                        shared_ptr<fholder> holder = fchker_.check(fargv->src(), max_effort);
                        if (holder.get() == NULL) {
                            if (!ignore_err) 
                                return ERROR_OpenFileFailed;
                            /// ignore error.
                            fargv->srcvalid(false);
                            LOG(WARNING) << "addf: holder failed! file://" << fargv->src();
                        } else {
                            fargv->srcvalid(true);
                            fcnter_.index(fargv->src());
                        }
                    }
                    break;
                /// xxx need add.
                default:
                    break;
                }
            }
        }

        return ERROR_Success;
    }

    void rm_bad_args()
    {
        std::vector<std::string> dstlist;

        LOG(INFO) << "rm bad args ---> before, arg list size(): " << arglist_.size();

        AutoArgvList::iterator it;
        for (it = arglist_.begin(); it != arglist_.end(); ) {
            if ((*it)->type() == kOut) {
                it = arglist_.erase(it);
                continue;
            }
            if ((*it)->type() == kAddf) {
                AddfArgv *farg = (AddfArgv*)((*it).get());
                if (!farg->srcvalid()) {
                    dstlist.push_back(farg->dst());
                    LOG(WARNING) << "addf: holder failed! dst file://" << farg->dst();
                    it = arglist_.erase(it);
                    continue;
                }
            }
            ++it;
        }

        LOG(INFO) << "rm bad args ---> addf dst number: " << dstlist.size();
        LOG(INFO) << "rm bad args ---> remove addf and out, arg list size(): " << arglist_.size();

        std::vector<std::string>::iterator dit;
        bool find;
        /// delete delf.
        for (it = arglist_.begin(); it != arglist_.end(); ) {
            if ((*it)->type() == kDelf) {
                DelfArgv *dargv = (DelfArgv *)((*it).get());
                find = false;
                for (dit = dstlist.begin(); dit != dstlist.end(); ++dit) {
                    if (dargv->dst() == (*dit)) {
                        find = true;
                        break;
                    }
                }
                if (find) {
                    it = arglist_.erase(it);

                    LOG(WARNING) << "addf: holder failed, delf remove: file://" << dargv->dst();

                    dstlist.erase(dit);
                    if (dstlist.size() == 0)
                        break;

                    continue;
                }
            }
            ++it;
        }

        LOG(INFO) << "rm bad args ---> remove delf, arg list size(): " << arglist_.size();
    }

    void ready()
    {
        // extract pkg argv string.
        std::vector<uint64_t> str_index;
        entry::transfer argvtransfer(acnter_, str_index);
        std::for_each(arglist_.begin(), arglist_.end(), argvtransfer);

        pkg_args_ = acnter_.map();

        for (size_t i = 0; i < pkg_args_.size(); ++i)
            DLOG(INFO) << "    args: " << pkg_args_[i];

        pkg_args_len_ = 0L;
        entry_t entry;

        pkg_files_.resize(fcnter_.cnt());

        for (size_t i = 0; i < arglist_.size(); ++i) {
            entry = helper::extract(arglist_[i]);
            entry.strindex = str_index[i];

            entry.dtaindex = kinvalid;
            /// if argv is file, file data index need calc.
            if (arglist_[i]->type() == kAddf) {
                AddfArgv *fargv = (AddfArgv *)arglist_[i].get();

                /// get file holder.
                shared_ptr<fholder> holder = fchker_.check(fargv->src());
                if (holder.get() == NULL) {
                    std::string error;
                    error += "Pkg: ready() >> check file \"";
                    error += fargv->src();
                    error += "\" error";
                    LOG(ERROR) << error;
                    throw pkg_error(ERROR_CheckFileError, error);
                }

                /// get file data index.
                uint64_t index;
                if ((index = fcnter_.index(fargv->src())) == kinvalid) {
                    std::string error;
                    error += "Pkg: ready() >> counter file \"";
                    error += fargv->src();
                    error += "\" error";
                    LOG(ERROR) << error;
                    throw pkg_error(ERROR_CheckFileError, error);
                }

                entry.dtaindex = index;

                /// add file holder to pkg holder list sequence.
                pkg_files_[(uint32_t)index] = holder;
            }
            
            pkg_entrys_.push_back(entry);
            pkg_args_len_ += pkg_args_[(uint32_t)str_index[i]].length() + 1;
        }
    }
#pragma endregion ready

#pragma region make

    uint64_t make_header(header_t *header)
    {
        header->magic = *(uint32_t *)(kmagic);
        header->crc32 = kinvalidcrc32;
        header->version  = kcurrentver;
        header->compress = kcompno;

        uint32_t entrys_num = pkg_entrys_.size() > 0 ? (pkg_entrys_.size() - 1) : 0;
        uint64_t args_len = pkg_args_len_ > 0L ? (pkg_args_len_ - 1) : 0L;
        uint32_t files_num = pkg_files_.size() > 0 ? (pkg_files_.size() - 1) : 0;

        header->entrymgr = sizeof(header_t);
        header->argvmgr  = header->entrymgr +
            sizeof(entrymgr_t) + entrys_num * sizeof(entry_t);
        header->dinfomgr = header->argvmgr + 
            sizeof(argvmgr_t) + args_len;
        header->data = header->dinfomgr + 
            sizeof(dinfomgr_t) + files_num * sizeof(dinfo_t);

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
        argvmgr->count = pkg_args_.size();
        argvmgr->length = pkg_args_len_;
        char *pos = argvmgr->start;
        for (size_t i = 0; i < pkg_args_.size(); ++i) {
            /// strcpy will copy null-terminal.
            strcpy(pos, pkg_args_[i].c_str());
            pos += pkg_args_[i].length() + 1;
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

#pragma endregion make pkg header

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

#endif // pkgtools_packer_h_