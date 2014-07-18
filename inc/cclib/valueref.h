//!
///

#if !defined(cclib_value_ref_h_)
#define cclib_value_ref_h_

#include "types.h"
#include "valueref.h"

CCLIB_NAMESPACE_BEGIN

template <typename T>
struct ValueRef
{   
    ValueRef(T& ref) : ref_(ref) {}
    ValueRef(const ValueRef& rhs) : ref_(rhs.ref_) {}
    operator T& () const { return ref_; }
private:
    ValueRef();
    ValueRef& operator=(const ValueRef&);

    T& ref_;
};

template <class T>
inline ValueRef<T> makeRef(T& t)
{
    return ValueRef<T>(t);
}  

CCLIB_NAMESPACE_END

#endif // cclib_value_ref_h_