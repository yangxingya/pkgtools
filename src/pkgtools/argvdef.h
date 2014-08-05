//!
///
///

#if !defined(pkgtools_argv_def_h_)
#define pkgtools_argv_def_h_

#include <string>
#include <vector>
#include <cclib/strutil.h>
#include <cclib/types.h>
#include <cclib/smartptr_def.h>
#include "pkgdef.h"

namespace argv {

using namespace cclib;
using namespace pkg;

struct ArgvBase;

typedef shared_ptr<ArgvBase>  AutoArgv;
typedef std::vector<AutoArgv> AutoArgvList;

const int kOut   = 1;
const int kAddf  = 2;
const int kDelf  = 3;
const int kMkdir = 4;
const int kRmdir = 5;
const int kExec  = 6;
const int kSetting = 7;

const int kUnknown = 0xffff;

#define k(x) k##x=""#x
const std::string k(out);
const std::string k(addf);
const std::string k(delf);
const std::string k(mkdir);
const std::string k(rmdir);
const std::string k(exec);
const std::string k(setting);
const std::string k(unknown);
#undef k

//!
/// detail:
///   flags:
///     7 6 5 4 3 2 1 0
///                   ^
///                  p/i/u
///                 ^
///                 t(error try?)
///               ^
///               e(error exit?)

const uint8_t kpkgtmask = 1;    /// 0x01
const uint8_t kinstmask = 1;    /// 0x01
const uint8_t kuninmask = 1;    /// 0x01  

const uint8_t kallmask = 7;     /// 0x07

/// error try mask
const uint8_t kerrtmask = 2;    /// 0x02
/// error exit mask
const uint8_t kerrxmask = 4;    /// 0x04

const std::string kargvbegin = "[";
const std::string kargvend = "]";
const std::string kseparate = " ";
const std::string kpkgtstr = "-p";
const std::string kinststr = "-i";
const std::string kuninstr = "-u";

/// no error try prefix, error try value follow '-p/-i/-u'.
/// detail: -p n/t, -i n/t, -u n/t
/// no error exit prefix likely '-e b/i', is only b/i. so:
/// detail: -p n/t b/i, -i n/t b/i, -u n/t b/i

const std::string knotrystr  = "n";
const std::string ktrystr    = "t";
const std::string kbreakstr  = "b";
const std::string kignorestr = "i";

/// error try value.
const uint8_t kerrtry = 2;
/// error no try value.
const uint8_t kerrnotry = 0;
/// error ignore value.
const uint8_t kerrignore = 4;
/// error break value.
const uint8_t kerrbreak = 0;
///!
/// the following is default value.
const uint8_t kdfttry = kerrtry; 
const uint8_t kdfterr = kerrignore;

const uint8_t kdftpkgt = 1;
const uint8_t kdftinst = 1;
const uint8_t kdftunin = 0;
const uint8_t kenabled = 1;

/// the following is all combined default flags.
const uint8_t kdftpcomb = kdftpkgt | kdfttry | kdfterr;
const uint8_t kdfticomb = kdftinst | kdfttry | kdfterr;
const uint8_t kdftucomb = kdftunin | kdfttry | kdfterr;

inline int type(std::string const& t)
{
    std::string ss = cclib::to_lower(t);
    if (ss == kout) return kOut;
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

inline uint8_t toInst(uint8_t entryflags) { return entryflags & kallmask; }
inline uint8_t toUnin(uint8_t entryflags) { return (entryflags >> kflagsuninshift) & kallmask; }
inline int toType(uint16_t entrytype) 
{
    switch (entrytype) {
    case kentryaddf:    return kAddf;
    case kentrydelf:    return kDelf;
    case kentrymkdir:   return kMkdir;
    case kentryrmdir:   return kRmdir;
    case kentryexec:    return kExec;
    case kentrysetting: return kSetting;
    default: return kUnknown;
    }
}

inline uint8_t toCheckRet(uint8_t entryflags) { return entryflags >> kflagsckshift; }

} // namespace argv

#endif // pkgtools_argv_def_h_