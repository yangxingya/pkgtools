//!
/// brief: api.h define win32 api 
///

#if !defined(cclib_win32_api_h_)
#define cclib_win32_api_h_

#ifndef WIN32_LEAN_AND_MEAN
# define WIN32_LEAN_AND_MEAN  // mini include <windows.h>
#endif // WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <string>
#include <stdexcept>  // for invalid_argument.
#include "../types.h"
#include "../nocopyable.h"
#include "../singleton.h"

#define WIN32_NAMESPACE win32
#define WIN32_NAMESPACE_BEGIN namespace win32 {
#define WIN32_NAMESPACE_END }

CCLIB_NAMESPACE_BEGIN
WIN32_NAMESPACE_BEGIN

const std::string kstrKernel = "kernel32.dll";
const std::string kstrUser32 = "user32.dll";
const std::string kstrNtdll = "ntdll.dll";
const std::string kstrShlwapi = "shlwapi.dll";
const std::string kstrAdvapi32 = "advapi32.dll";

const std::string kstrUnknown = "<unknown.dll>";
///TODO:: add.

/// kernel32.dll export function.
const std::string kfnDisableWow64FSRedirect = "Wow64DisableWow64FsRedirection";
typedef BOOL (__stdcall *Wow64FsRedirectDisabler)(void **);
const std::string kfnEnableWow64FSRedirect = "Wow64RevertWow64FsRedirection";
typedef BOOL (__stdcall *Wow64FsRedirectEnabler)(void *);

enum ApiModule {
    ModKernel32 = 1,
    ModUser32   = 2,
    ModNtdll    = 3,
    ModShlwapi  = 4,
    ModAdvapi32 = 5,
};

inline std::string makeName(ApiModule module)
{
    switch (module) {
    case ModKernel32: return kstrKernel;
    case ModUser32:   return kstrUser32;
    case ModNtdll:    return kstrNtdll;
    case ModShlwapi:  return kstrShlwapi;
    case ModAdvapi32: return kstrAdvapi32;
    default:          return kstrUnknown;
    }
}

//!
/// brief: DllLoader define dll loader
/// usage: .cotr will throw invalid_argument except.
/// 
struct DllLoader : public nocopyable
{   
    DllLoader(std::string const &name = kstrKernel)
        : loaded_(false)
    {
        hmod_ = GetModuleHandleA(name.c_str());
        if (hmod_) 
            return;

        hmod_ = LoadLibraryA(name.c_str());
        if (hmod_ == NULL) {
            std::string error = "DllLoader .cotr arg: '";
            error += name;
            error += "' invalided!";
            throw std::invalid_argument(error);
        }
        loaded_ = true;
    }

    ~DllLoader() 
    {
        if (loaded_)
            FreeLibrary(hmod_);
    }

    template<typename Func>
    Func find(std::string const &funcname)
    {
        return reinterpret_cast<Func>(GetProcAddress(hmod_, funcname.c_str()));
    }

private:
    HMODULE hmod_;
    bool loaded_;
};

template<ApiModule mod = ModKernel32>
struct SystemDll
{
    SystemDll() : loaders_(makeName(mod)) {}

    template<typename Func>
    Func find(std::string const& funcname)
    {
        return loaders_.find<Func>(funcname);
    }
private:
    DllLoader loaders_;
};

inline SystemDll<ModKernel32>& kernel32Dll() { return singleton<SystemDll<ModKernel32> >::ref(); }
inline SystemDll<ModUser32>&   user32Dll()   { return singleton<SystemDll<ModUser32> >::ref();   }
inline SystemDll<ModNtdll>&    ntdllDll()    { return singleton<SystemDll<ModNtdll> >::ref();    }
inline SystemDll<ModShlwapi>&  shlwapiDll()  { return singleton<SystemDll<ModShlwapi> >::ref();  }
inline SystemDll<ModAdvapi32>& advapi32Dll() { return singleton<SystemDll<ModAdvapi32> >::ref(); }

WIN32_NAMESPACE_END
CCLIB_NAMESPACE_END

#endif // cclib_win32_api_h_