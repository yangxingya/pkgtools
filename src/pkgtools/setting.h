//!
/// brief: setting.h define global setting.
///        setting is only be set before package/install/uninstall timepoint.
///

#if !defined(pkgtools_setting_h_)
#define pkgtools_setting_h_

#include <string>
#include <cclib/strutil.h>
#include <cclib/win32/disabler.h>
#include "pkgdef.h"
#include "error.h"
#include "except.h"

namespace pkg {

using namespace cclib;
using namespace argv;

const std::string kdisfsredirectstr = "dfsrd";
const std::string kenbfsredirectstr = "efsrd";

const int kdisablefsredirection = 1;
const int kenablefsredirection = 2;
const int kinvalidargs = -1;

struct Setting
{
    Setting(std::string const& argv)
        : argv_(argv)
    {
        parse();
    }

    static int valid(std::string const& argv)
    {
        /// supported cmdline args: dfsrd/efsrd
        std::string largv = to_lower(argv);

        if (largv == kdisfsredirectstr)
            return kdisablefsredirection;

        if (largv == kenbfsredirectstr)
            return kenablefsredirection;

        ///todo add supported args.

        return kinvalidargs;
    }

    int doSet()
    {
        switch (detail_) {
        case kdisablefsredirection: 
            disableFSRedirection();
            break;
        case kenablefsredirection:
            enableFSRedirection();
            break;

        /// todo add new supported setting.


        default:
            LOG(ERROR) << "Setting, not supported cmd!";
            return ERROR_SettingArgsNotSupported;
        }

        return ERROR_Success;
    }

private:
    std::string argv_;
    int detail_;
    

    void parse()
    {
        detail_ = valid(argv_);
        if (detail_ == kinvalidargs) {
            /// error, not supported args.
            std::string error = "Setting parse args: \"";
            error += argv_;
            error += "\" not supported";

            LOG(ERROR) << error;
            throw setting_error(ERROR_SettingArgsNotSupported, error);
        }

    }

    void disableFSRedirection()
    {
        win32::Wow64FileSystem fs;
        fs.disable();
    }

    void enableFSRedirection()
    {
        win32::Wow64FileSystem fs;
        fs.enable();
    }
};

} // namespace pkg

#endif // pkgtools_setting_h_