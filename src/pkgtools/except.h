//! except.h is define all throwed exception
///
///

#if !defined(pkgtools_except_h_)
#define pkgtools_except_h_

#include <string>

struct except_base 
{
    except_base(int errcode, std::string const& what)
        : errcode_(errcode), what_(what) {}
    virtual ~except_base(){}
    const char* what() const { return what_.c_str(); }
    int error() const { return errcode_; }
protected:
    std::string what_;
    int errcode_;
};

struct pkg_error : public except_base
{
    pkg_error(int errcode, std::string const& what = "make package error")
        : except_base(errcode, what) {}
};

struct inst_error : public except_base
{
    inst_error(int errcode, std::string const& what = "install error")
        : except_base(errcode, what) {}
};





#endif // pkgtools_except_h_