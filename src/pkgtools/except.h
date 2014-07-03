//! except.h is define all throwed exception
///
///

#if !defined(pkgtools_except_h_)
#define pkgtools_except_h_

#include <string>

struct except_base 
{
    virtual ~except_base(){}
    const char* what() const { return what_.c_str(); }
protected:
    std::string what_;
};





#endif // pkgtools_except_h_