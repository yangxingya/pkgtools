//!
/// brief: installer.h define install a package to pc.
///

#if !defined(pkgtools_installer_h_)
#define pkgtools_installer_h_

#include <string>
#include <cclib/types.h>
#include <cclib/win32/executer.h>
#include "unpacker.h"
#include "pkgfile.h"
#include "argv.h"
#include "argvdef.h"

namespace pkg {

using namespace cclib;
using namespace argv;

struct Installer
{
    Installer(std::string const& pkgfile)
        : unpacker_(pkgfile) {}

    int install()
    {
        /// first: get unpacker entrys and arglist.
        std::vector<entry_t> const& entrys = unpacker_.entrys();
        std::vector<std::string> const& args = unpacker_.arglist();

        /// second: restore arglist to current dir.
        AutoArgvList arglist;
        entry::restorer restor(args, arglist);
        std::for_each(entrys.begin(), entrys.end(), restor);
    
        /// third: install every entry sequencely.
        for (size_t i = 0L; i < arglist.size(); ++i) {
            switch (arglist[i]->type()) {
            case entry::kFile:
                {
                    FileArgv *fargv = (FileArgv*)arglist[i].get();
                    if (fargv->instFlags() != 0) {
                        unpacker_.tofile(fargv->dst(), fargv->offset());
                        LOG(INFO) << "Install: unpack file to: \"" << fargv->dst() << "\" successful!";
                    }
                    break;
                }
            case entry::kDir:
                {
                    DirArgv *dargv = (DirArgv*)arglist[i].get();
                    if (dargv->instFlags() != 0) {
                        if (win32::mkdirtree(dargv->dst()))
                            LOG(INFO) << "Install: create dir: \"" << dargv->dst() << "\" successful!";
                        else
                            LOG(WARNING) << "Install: create dir: \"" << dargv->dst() << "\" failed!";
                    }
                    break;
                }
            case entry::kExec:
                {
                    ExecArgv *eargv = (ExecArgv*)arglist[i].get();

                    win32::execute("", eargv->cmd());

                    LOG(INFO) << "exec entry: \"" << eargv->cmd() << "\" not exec";
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

#endif // pkgtools_installer_h_