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

struct no_impl : public except_base
{
    no_impl(int errcode = -1, std::string const& what = "not implement")
        : except_base(errcode, what) {}
};

struct script_error : public except_base
{
    script_error(int errcode = -1, std::string const& what = "script format error")
        : except_base(errcode, what) {}
};

struct pkg_error : public except_base
{
    pkg_error(int errcode = -1, std::string const& what = "make package error")
        : except_base(errcode, what) {}
};

struct inst_error : public except_base
{
    inst_error(int errcode = -1, std::string const& what = "install error")
        : except_base(errcode, what) {}
};

struct uninst_error : public except_base
{
    uninst_error(int errcode = -1, std::string const& what = "uninstall error")
        : except_base(errcode, what) {}
};

struct extract_error : public except_base
{
    extract_error(int errcode = -1, std::string const& what = "extract package error")
        : except_base(errcode, what) {}
};

struct spget_error : public except_base
{
    spget_error(int errcode = -1, std::string const& what = "special folder get error")
        : except_base(errcode, what) {}
};

#endif // pkgtools_except_h_