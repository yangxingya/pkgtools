//!
/// brief: entrydef.h define entry define
///

#if !defined(pkgtools_entry_def_h_)
#define pkgtools_entry_def_h_

#include <string>
#include <vector>
#include <cclib/strutil.h>

namespace entry {

const int kOut  = 1;
const int kFile = 2;
const int kDir  = 3;
const int kExec = 4;
const int kSetting = 5;

const int kUnknown = 0xffff;

#define k(x) k##x=""#x
const std::string k(out);
const std::string k(file);
const std::string k(dir);
const std::string k(exec);
const std::string k(unknown);
const std::string k(setting);
#undef k

inline int type(std::string const& t)
{
    std::string ss = cclib::to_lower(t);
    if (ss == kout) return kOut;
    else
    if (ss == kfile) return kFile;
    else
    if (ss == kdir) return kDir;
    else
    if (ss == kexec) return kExec;
    else
    if (ss == ksetting) return kSetting;
    return kUnknown;
}

} // namespace entry

#endif // pkgtools_entry_def_h_