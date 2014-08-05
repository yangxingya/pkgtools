//!
/// brief: installer.h define install a package to pc.
///

#if !defined(pkgtools_installer_h_)
#define pkgtools_installer_h_

#include <string>
#include <cclib/types.h>
#include <cclib/strutil.h>
#include <cclib/win32/executer.h>
#include "unpacker.h"
#include "runner.h"

namespace pkg {

using namespace cclib;
using namespace argv;

struct Installer
{
    Installer(std::string const& pkgfile, std::string const& arglist)
        : unpacker_(pkgfile), arglist_(arglist) {}

    int install()
    {
        std::map<std::string, std::string> arg_map;

        LOG(INFO) << "Install args: \"" << arglist_ << "\"";
        split(arglist_, " ", "=", &arg_map);

        /// first: get unpacker entrys and arglist.
        std::vector<entry_t> const& entrys = unpacker_.entrys();
        std::vector<std::string> const& args = unpacker_.arglist();

        /// second: restore arglist to current dir.
        AutoArgvList arglist;
        entry::restorer restor(args, arglist);
        std::for_each(entrys.begin(), entrys.end(), restor);

        runner runner(unpacker_, OP_Install, arg_map);
        std::for_each(arglist.begin(), arglist.end(), runner);

        return ERROR_Success;
    }

private:
    unpacker unpacker_;
    std::string arglist_;
};

} // namespace pkg

#endif // pkgtools_installer_h_