//!
/// brief: autores.h is auto resource manager.
/// usage: auto manage is that: use RAII(Resource Acquisition Is Initialization) idioms
///        to manage resource, not to explicit release resource. to avoid resource not
///        released leak.

#if !defined(cclib_win32_auto_resource_h_)
#define cclib_win32_auto_resource_h_

#include <windows.h>
#include "../types.h"
#include "../scopeguard.h"

CCLIB_NAMESPACE_BEGIN
WIN32_NAMESPACE_BEGIN

#define ScopeHandle(x) ScopeGuard VARNAME_WITH_LINE(guard) = makeGuard(CloseHandle, x)
#define ScopeHwnd(wnd) ScopeGuard VARNAME_WITH_LINE(guard) = makeGuard(DestroyWindow, wnd)

template<typename Fun, typename Para>
struct AutoResource
{
    AutoResource(Fun fun = 0, Para p = 0)
        : fun_(fun), p_(p) {}

    ~AutoResource()
    {
        if (fun_) 
            fun_(p_);
    }

private:
    Fun fun_;
    Para p_;
};

/// define auto resource manager.
#define Auto(R, F)                      \
    struct Auto ## R                    \
    {                                   \
        Auto ## R(R r = 0) : r_(r) {}   \
        ~Auto ## R() { reset(); }       \
        R get() const { return r_; }    \
        void reset(R r = 0)             \
        {                               \
            if (r_) F(r_);              \
            r_ = r;                     \
        }                               \
    private:                            \
        R r_;                           \
    };
    
Auto(HANDLE, CloseHandle);
Auto(HWND, DestroyWindow);

WIN32_NAMESPACE_END
CCLIB_NAMESPACE_END

#endif // cclib_win32_auto_resource_h_