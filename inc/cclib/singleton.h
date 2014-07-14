//!
/// brief: singleton.h define singleton 
///

#if !defined(cclib_singleton_h_)
#define cclib_singleton_h_

#include "types.h"
#include "nocopyable.h"

CCLIB_NAMESPACE_BEGIN

template<typename T>
struct singleton : public nocopyable
{
    static T& ref() 
    {
        static T t;
        return t;
    }

    static T* pointer()
    {
        static T *pt = new T;
        ///HACK: can improve it use double-check lock.
        ///     if it need use in thread-safed environment.
        return pt;
    }
    ~singleton() {}
private:
    singleton() {}
};

CCLIB_NAMESPACE_END

#endif // cclib_singleton_h_