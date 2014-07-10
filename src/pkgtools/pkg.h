//!
/// brief: pkg.h is defined for package usage.
///

#if !defined(pkgtools_pkg_h_)
#define pkgtools_pkg_h_

#include "argvdef.h"

struct PkgBase 
{
    virtual int doPkg() = 0;
    virtual ~PkgBase() {}
};

struct PkgOut : public PkgBase
{
    PkgOut(AutoArgv argv);
    virtual int doPkg();
    std::string get() const { return out_; }
private:
    std::string out_;
};

struct PkgFile : public PkgBase 
{
    PkgFile(AutoArgv argv);
    virtual int doPkg();
    std::string get() const { return to_; }
};

#endif // pkgtools_pkg_h_