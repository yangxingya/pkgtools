//!
///
///

#if !defined(pkgtools_argv_def_h_)
#define pkgtools_argv_def_h_

#include <vector>
#include <cclib/smartptr_def.h>

namespace argv {

struct ArgvBase;

typedef shared_ptr<ArgvBase> AutoArgv;
typedef std::vector<AutoArgv> AutoArgvList;

} // namespace argv

#endif // pkgtools_argv_def_h_