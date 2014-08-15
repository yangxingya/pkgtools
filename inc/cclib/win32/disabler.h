//!
/// brief: disabler.h for disable something, 
///        for example: wow64 exe filesystem redirection
///        disable.

#if !defined(cclib_win32_disabler_h_)
#define cclib_win32_disabler_h_

#include "../nocopyable.h"
#include "api.h"

CCLIB_NAMESPACE_BEGIN
WIN32_NAMESPACE_BEGIN

template<typename T>
struct disabler : public nocopyable
{
    explicit disabler(T &t) : t_(t) { t_.disable(); }
    ~disabler() { t_.enable(); }
private:
    T &t_;
};

struct Wow64FileSystem
{
    Wow64FileSystem() 
    {
    
    }
    bool disable() 
    {
        Wow64FsRedirectDisabler disabler 
            = kernel32Dll().find<Wow64FsRedirectDisabler>(kfnDisableWow64FSRedirect);
        
        /// win vista or later have this function
        if (disabler)
            return !!disabler(&ori_value_);

        /// win xp not the function
        return true;
    }
    bool enable()
    {
        Wow64FsRedirectEnabler enabler
            = kernel32Dll().find<Wow64FsRedirectEnabler>(kfnEnableWow64FSRedirect);
        
        /// win vista or later have this function
        if (enabler) 
            return !!enabler(ori_value_);

        /// win xp not the function
        return true;
    }

private:
    void *ori_value_;
};

WIN32_NAMESPACE_END
CCLIB_NAMESPACE_END

#endif // cclib_win32_disabler_h_