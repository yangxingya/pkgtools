//!
/// brief: installer.h define install a package to pc.
///

#if !defined(pkgtools_installer_h_)
#define pkgtools_installer_h_

#include <string>
#include <cclib/types.h>
#include "extractor.h"
#include "pkgfile.h"

namespace pkg {

using namespace cclib;

struct Installer
{
    Installer(std::string const& pkgfile)
        : reader_(pkgfile) {}

private:
    Reader reader_;
};

} // namespace pkg

#endif // pkgtools_installer_h_