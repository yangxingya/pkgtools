//!
/// brief: pkgextractor.h define package extractor.
///

#if !defined(pkgtools_pkg_extractor_h_)
#define pkgtools_pkg_extractor_h_

#include <string>
#include <cclib/types.h>
#include <cclib/win32/pathutil.h>
#include <glog/logging.h>
#include "pkgfile.h"
#include "pkgdef.h"
#include "unpacker.h"
#include "argvtrans.h"
#include "argvdef.h"
#include "argv.h"

namespace pkg {

using namespace cclib;
using namespace argv;

struct Extractor
{
    Extractor(std::string const& pkgfile, std::string const& todir)
        : unpacker_(pkgfile), todir_(todir) 
    {
    }

    int extract()
    {
        std::vector<entry_t> const& entrys = unpacker_.entrys();
        std::vector<std::string> const& args = unpacker_.arglist();

        AutoArgvList arglist;
        entry::restorer restor(args, arglist, true, todir_);
        std::for_each(entrys.begin(), entrys.end(), restor);
    
        for (size_t i = 0L; i < arglist.size(); ++i) {
            switch (arglist[i]->type()) {
            case entry::kFile:
                {
                    FileArgv *fargv = (FileArgv*)arglist[i].get();
                    unpacker_.tofile(fargv->dst(), fargv->offset());
                    break;
                }
            case entry::kDir:
                {
                    DirArgv *dargv = (DirArgv*)arglist[i].get();
                    win32::mkdirtree(dargv->dst());
                    break;
                }
            case entry::kExec:
                {
                    ExecArgv *eargv = (ExecArgv*)arglist[i].get();
                    LOG(INFO) << "Extractor, exec entry: \"" << eargv->cmd() << "\" not exec";
                    break;
                }
            }
        }
        return ERROR_Success;
    }

private:
    unpacker unpacker_;
    std::string todir_;
};

} // namespace pkg

#endif // pkgtools_pkg_extractor_h_