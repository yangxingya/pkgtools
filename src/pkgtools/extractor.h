//!
/// brief: pkgextractor.h define package extractor.
///

#if !defined(pkgtools_pkg_extractor_h_)
#define pkgtools_pkg_extractor_h_

#include <string>
#include <cclib/types.h>
#include <cclib/win32/pathutil.h>
#include <glog/logging.h>
#include "unpacker.h"
#include "runner.h"


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
    
        std::map<std::string, std::string> arg_map;
        runner runner(unpacker_, OP_Extract, arg_map);
        std::for_each(arglist.begin(), arglist.end(), runner);

        return ERROR_Success;
    }

private:
    unpacker unpacker_;
    std::string todir_;
};

} // namespace pkg

#endif // pkgtools_pkg_extractor_h_