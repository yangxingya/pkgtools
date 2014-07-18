//!
/// brief: autores.h is auto resource manager.
/// usage: auto manage is that: use RAII(Resource Acquisition Is Initialization) idioms
///        to manage resource, not to explicit release resource. to avoid resource not
///        released leak.

#if !defined(cclib_win32_auto_resource_h_)
#define cclib_win32_auto_resource_h_

#include "../types.h"
#include "../scopeguard.h"

CCLIB_NAMESPACE_BEGIN
WIN32_NAMESPACE_BEGIN

#define ScopeHandle(x) ScopeGuard VARNAME_WITH_LINE(guard) = makeGuard(CloseHandle, x)
#define ScopeHwnd(wnd) ScopeGuard VARNAME_WITH_LINE(guard) = makeGuard(DestroyWindow, wnd)

WIN32_NAMESPACE_END
CCLIB_NAMESPACE_END

#endif // cclib_win32_auto_resource_h_