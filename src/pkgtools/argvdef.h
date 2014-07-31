//!
///
///

#if !defined(pkgtools_argv_def_h_)
#define pkgtools_argv_def_h_

#include <string>
#include <vector>
#include <cclib/strutil.h>
#include <cclib/smartptr_def.h>

namespace argv {

struct ArgvBase;

typedef shared_ptr<ArgvBase>  AutoArgv;
typedef std::vector<AutoArgv> AutoArgvList;

const int kOut   = 1;
const int kFile  = 2;
const int kAddf  = 3;
const int kDelf  = 4;
const int kMkdir = 5;
const int kRmdir = 6;
const int kExec  = 7;
const int kSetting = 8;

const int kUnknown = 0xffff;

#define k(x) k##x=""#x
const std::string k(out);
const std::string k(file);
const std::string k(addf);
const std::string k(delf);
const std::string k(mkdir);
const std::string k(rmdir);
const std::string k(exec);
const std::string k(setting);
const std::string k(unknown);
#undef k

inline int type(std::string const& t)
{
    std::string ss = cclib::to_lower(t);
    if (ss == kout) return kOut;
    else
    if (ss == kfile) return kFile;
    else
    if (ss == kaddf) return kAddf;
    else
    if (ss == kdelf) return kDelf;
    else
    if (ss == kmkdir) return kMkdir;
    else
    if (ss == krmdir) return kRmdir;
    else
    if (ss == kexec) return kExec;
    else
    if (ss == ksetting) return kSetting;
    return kUnknown;
}


} // namespace argv

#endif // pkgtools_argv_def_h_