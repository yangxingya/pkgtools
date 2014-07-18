//!
/// brief: uninstaller.h define uninstall a installed software.
///

#if !defined(pkgtools_uninstaller_h_)
#define pkgtools_uninstaller_h_

#include <string>
#include <cclib/types.h>
#include "unpacker.h"
#include "pkgfile.h"
#include "argv.h"
#include "argvdef.h"

namespace pkg {

using namespace cclib;
using namespace argv;

struct Uninstaller
{
    Uninstaller(std::string const& pkgfile)
        : unpacker_(pkgfile)
    { }

    int uninstall()
    {
        std::vector<entry_t> const& entrys = unpacker_.entrys();
        std::vector<std::string> const& args = unpacker_.arglist();

        AutoArgvList arglist;
        entry::restorer restor(args, arglist);
        std::for_each(entrys.begin(), entrys.end(), restor);
    
        for (size_t i = 0; i < arglist.size(); ++i) {
            switch (arglist[arglist.size() - 1 - i]->type()) {
            case entry::kFile:
                {
                    FileArgv *fargv = (FileArgv*)arglist[arglist.size() - 1 - i].get();
                    if (fargv->uninFlags() != 0) {
                        if (win32::rmfile(fargv->dst()))
                            LOG(INFO) << "Uninstall: remove file: \"" << fargv->dst() << "\" successful!";
                        else
                            LOG(WARNING) << "Uninstall: remove file: \"" << fargv->dst() << "\" failed!";
                    }
                    break;
                }
            case entry::kDir:
                {
                    DirArgv *dargv = (DirArgv*)arglist[arglist.size() - 1 - i].get();
                    if (dargv->uninFlags() != 0) {
                        if (win32::rmdir(dargv->dst()))
                            LOG(INFO) << "Uninstall: remove dir: \"" << dargv->dst() << "\" successful!";
                        else
                            LOG(WARNING) << "Uninstall: remove dir: \"" << dargv->dst() << "\" failed!";
                    }
                    break;
                }
            case entry::kExec:
                {
                    ExecArgv *eargv = (ExecArgv*)arglist[arglist.size() - 1 - i].get();
                    LOG(INFO) << "Uninstall: exec entry: \"" << eargv->cmd() << "\" not exec";
                    break;
                }
            }
        }
        return ERROR_Success;
    }

private:
    unpacker unpacker_;
};

} // namespace pkg

#endif // pkgtools_uninstaller_h_