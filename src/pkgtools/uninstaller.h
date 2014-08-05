//!
/// brief: uninstaller.h define uninstall a installed software.
///

#if !defined(pkgtools_uninstaller_h_)
#define pkgtools_uninstaller_h_

#include <string>
#include <cclib/types.h>
#include "unpacker.h"
#include "runner.h"

namespace pkg {

using namespace cclib;
using namespace argv;

struct Uninstaller
{
    Uninstaller(std::string const& pkgfile, std::string const& arglist)
        : unpacker_(pkgfile), arglist_(arglist) {}

    int uninstall()
    {
        std::map<std::string, std::string> arg_map;

        LOG(INFO) << "Install args: \"" << arglist_ << "\"";
        split(arglist_, " ", "=", &arg_map);

        std::vector<entry_t> const& entrys = unpacker_.entrys();
        std::vector<std::string> const& args = unpacker_.arglist();

        AutoArgvList arglist;
        entry::restorer restor(args, arglist);
        std::for_each(entrys.begin(), entrys.end(), restor);
    
        runner runner(unpacker_, OP_Uninstall, arg_map);
        std::for_each(arglist.begin(), arglist.end(), runner);

        return ERROR_Success;
    }

private:
    unpacker unpacker_;
    std::string arglist_;
};

} // namespace pkg

#endif // pkgtools_uninstaller_h_