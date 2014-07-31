//!
/// brief: packer.h define pack installed package api.
///

#if !defined(pkgtools_packer_h_)
#define pkgtools_packer_h_

#include <string>
#include "error.h"
#include "except.h"
#include "fchecker.h"
#include "counter.h"
#include "argv.h"


namespace pkg {

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
        
    }

private:
    AutoArgvList arglist_;
    std::string out_;

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

    //!
    /// brief: pre-pack.
    int prepack()
    {
        /// file open checker.
        /// if file open check failed, then return failed.
        ArgvBase *arg;
        for (size_t i = 0; i < arglist_.size(); ++i) {
            if (arglist_[i]->hasPkgt()) {
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
    }

    void rmPrePackArgs()
    {
        std::vector<std::string> dstlist;

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
                    it = arglist_.erase(it);
                    continue;
                }
            }
            ++it;
        }

        std::vector<std::string>::iterator dit;
        bool find;
        /// delete delf.
        for (it = arglist_.begin(); it != arglist_.end(); ) {
            if ((*it)->type() == kDelf) {
                DelfArgv *dargv = (DelfArgv *)((*it).get());
                find = false;
                for (dit = dstlist.begin(); dit != dstlist.end(); ++dit) {
                    if (dargv->dst() == dit) {
                        find = true;
                        break;
                    }
                }
                if (find)
                    dstlist.erase(dit);

                if (dstlist.size() == 0)
                    break;
            }
        }
    }
    
};

} // namespace pkg

#endif // pkgtools_packer_h_