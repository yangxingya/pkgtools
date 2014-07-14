//!
/// brief: nocopyable.h define concept: obj can't copy and assign.
///

#if !defined(cclib_nocopyable_h_)
#define cclib_nocopyable_h_

#include "types.h"

CCLIB_NAMESPACE_BEGIN
//!
/// brief: nocopyable obj
/// usage: 
///     class myclass : public nocopyable {
///         ...;
///     };
///
///     then: myclass can't support copy and 
///     assignment .cotr.
struct nocopyable
{
protected:
    nocopyable() {}
    ~nocopyable() {}
private:
    nocopyable(nocopyable const&);
    nocopyable& operator=(nocopyable const&);
};

/// brief: disallow copy and assign macro
///     nocopyable can be replace by it.
/// usage:
///     class myclass { 
///         ...;
///         DISALLOWCOPYANDASSIGN(myclass); 
///     };
///
///     then you can't use copy and assign .cotr.
///
#define DISALLOWCOPYANDASSIGN(cls) \
    private:\
        cls(cls const&); \
        cls& operator=(cls const&)


CCLIB_NAMESPACE_END

#endif // cclib_nocopyable_h_