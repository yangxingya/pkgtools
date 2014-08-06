//
// @brief: shadow.h wrapper for use.
// @usage:
//        UNICODE std::string:
//          ShadowCopyHelperW shadow(L"D:\\");
//          if (shadow.ready())
//              //succeed... you can get volume map address.
//              shadow.shadowPath();
//          else
//              //failed... you can check error std::string.
//              shadow.error(); 
//
//        ASCII std::string:
//          ShadowCopyHelperA shadow("D:\\");
//          if (shadow.ready())
//              //succeed... you can get volume map address.
//              shadow.shadowPath();
//          else
//              //failed... you can check error std::string.
//              shadow.error(); 
//

#ifndef VSS_COPYER_H_
#define VSS_COPYER_H_

#include <string>
#include <shadow.h>
#include <cclib/singleton.h>
#include <cclib/strutil.h>
#include <cclib/smartptr_def.h>
#include <cclib/win32/osinfo.h>
#include <cclib/win32/pathutil.h>

inline bool COMInitialize() { return CoInitialize(NULL) == S_OK; }

#if VSS_SHARED

struct ShadowCopyHelperW {
public:
    //
    // @brief : constructor.
    // @param : volume-> need vss volume, UNICODE std::string.
    // @param : dll_name-> shadow dll name, UNICODE std::string.
    // @return: you can check ready() function value to test vss copy ok.
    //
    ShadowCopyHelperW(std::wstring const &volume, std::wstring const &dll_name = L"shadow.dll")
        : module_ (LoadLibraryW(dll_name.c_str()))
        , create_ ((Fun_CreateShadowCopy )GetProcAddress(module_, FunName_CreateShadowCopy))
        , release_((Fun_ReleaseShadowCopy)GetProcAddress(module_, FunName_ReleaseShadowCopy))
        , getpath_((Fun_GetShadowCopyPath)GetProcAddress(module_, FunName_GetShadowCopyPath))
        , lasterr_((Fun_LastError        )GetProcAddress(module_, FunName_LastError))
        , handle_ (NULL)
        , error_  (L"NO Initialize")
    {
        ready_ = 
            module_  != NULL &&
            create_  != NULL &&
            release_ != NULL &&
            getpath_ != NULL &&
            lasterr_ != NULL;
        if (ready_) {
            handle_ = create_(volume.c_str());
            error_  = lasterr_();
            ready_  = handle_ != NULL;
        }
    }

    ~ShadowCopyHelperW() 
    { 
        if (handle_) if (release_) release_(handle_);
        if (module_) FreeLibrary(module_); 
    }
    bool ready() const { return ready_; }
    const wchar_t* error() const { return error_.c_str(); }
    const wchar_t* shadowPath() const 
    { 
        if (ready_) return getpath_(handle_); 
        return L"";
    }
private:
    HMODULE module_;
    Fun_CreateShadowCopy  create_;
    Fun_ReleaseShadowCopy release_;
    Fun_GetShadowCopyPath getpath_;
    Fun_LastError         lasterr_;
    bool ready_;
    VssHandle handle_;
    std::wstring error_;
};

struct ShadowCopyHelperA {
public:
    //
    // @brief : constructor.
    // @param : volume-> need vss volume, ASCII std::string.
    // @param : dll_name-> shadow dll name, ASCII std::string.
    // @return: you can check ready() function value to test vss copy ok.
    //
    ShadowCopyHelperA(std::string const &volume, std::string const &dll_name = "shadow.dll")
        : wvolume_(str2wstr(volume)), wdll_name_(str2wstr(dll_name))
        , helper_(wvolume_, wdll_name_)
    {
        const char *errstr = wstr2str(helper_.error());
        error_ = errstr;
        free((void*)errstr);
    }

    ~ShadowCopyHelperA() 
    { 
        if (wvolume_  ) free((void*)wvolume_);
        if (wdll_name_) free((void*)wdll_name_);
    }
    bool ready() const { return helper_.ready(); }
    const char* error() const { return error_.c_str(); }
    const char* shadowPath() const 
    { 
        const char *path = wstr2str(helper_.shadowPath());
        path_ = path;
        free((void*)path);
        return path_.c_str();
    }
private:
    const wchar_t *wvolume_;
    const wchar_t *wdll_name_;
    ShadowCopyHelperW helper_;
    std::string error_;
    mutable std::string path_;

    // ascii std::string to wide std::string .
    static const wchar_t* str2wstr(std::string const &str)
    {
        int len = MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, NULL, 0);
        wchar_t *wstr = (wchar_t*)malloc(len * sizeof(wchar_t));
        MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, wstr, len);
        return wstr;
    }
    static const char* wstr2str(std::wstring const &wstr)
    {
        int len = WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), -1, NULL, 0, 0, 0);
        char *str = (char*)malloc(len);
        WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), -1, str, len, 0, 0);
        return str;
    }

    friend std::string shadowDllNameA();
};

inline std::wstring shadowDllNameW() 
{
    using namespace cclib;

    std::wstring shadow = L"shadow";
    std::wstring winver;
    if (win32::win7()) 
        winver = L"win7";
    else if (win32::win2003())
        winver = L"win2003";
    else // if (winXpOrLater())
        winver = L"winxp";

    shadow += L"_";
    shadow += winver;
    
    std::wstring art = L"x86";
    if (sizeof(void*) == 8)
        art = L"x64";

    shadow += L"_";
    shadow += art;

    shadow += L".dll";

    return shadow;
}

inline std::string shadowDllNameA() 
{
    std::wstring wshadow = shadowDllNameW();
    std::string  shadow  = ShadowCopyHelperA::wstr2str(wshadow);
    return shadow;
}

#else // dll shared lib.

struct ShadowCopyHelperW {
public:
    //
    // @brief : constructor.
    // @param : volume-> need vss volume, UNICODE std::string.
    // @return: you can check ready() function value to test vss copy ok.
    //
    ShadowCopyHelperW(std::wstring const &volume)
        : create_ (CreateShadowCopy)
        , release_(ReleaseShadowCopy)
        , getpath_(GetShadowCopyPath)
        , lasterr_(LastError)
        , handle_ (NULL)
        , error_  (L"NO Initialize")
    {
        ready_ = 
            create_  != NULL &&
            release_ != NULL &&
            getpath_ != NULL &&
            lasterr_ != NULL;
        if (ready_) {
            handle_ = create_(volume.c_str());
            error_  = lasterr_();
            ready_  = handle_ != NULL;
        }
    }

    ~ShadowCopyHelperW() 
    { 
        if (handle_) if (release_) release_(handle_);
    }
    bool ready() const { return ready_; }
    const wchar_t* error() const { return error_.c_str(); }
    const wchar_t* shadowPath() const 
    { 
        if (ready_) return getpath_(handle_); 
        return L"";
    }
private:
    Fun_CreateShadowCopy  create_;
    Fun_ReleaseShadowCopy release_;
    Fun_GetShadowCopyPath getpath_;
    Fun_LastError         lasterr_;
    bool ready_;
    VssHandle handle_;
    std::wstring error_;
};

struct ShadowCopyHelperA {
public:
    //
    // @brief : constructor.
    // @param : volume-> need vss volume, ASCII std::string.
    // @return: you can check ready() function value to test vss copy ok.
    //
    ShadowCopyHelperA(std::string const &volume)
        : wvolume_(str2wstr(volume))
        , helper_(wvolume_)
    {
        const char *errstr = wstr2str(helper_.error());
        error_ = errstr;
        free((void*)errstr);
    }

    ~ShadowCopyHelperA() 
    { 
        if (wvolume_  ) free((void*)wvolume_);
    }
    bool ready() const { return helper_.ready(); }
    const char* error() const { return error_.c_str(); }
    const char* shadowPath() const 
    { 
        const char *path = wstr2str(helper_.shadowPath());
        path_ = path;
        free((void*)path);
        return path_.c_str();
    }
private:
    const wchar_t *wvolume_;
    ShadowCopyHelperW helper_;
    std::string error_;
    mutable std::string path_;

    // ascii std::string to wide std::string .
    static const wchar_t* str2wstr(std::string const &str)
    {
        int len = MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, NULL, 0);
        wchar_t *wstr = (wchar_t*)malloc(len * sizeof(wchar_t));
        MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, wstr, len);
        return wstr;
    }
    static const char* wstr2str(std::wstring const &wstr)
    {
        int len = WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), -1, NULL, 0, 0, 0);
        char *str = (char*)malloc(len);
        WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), -1, str, len, 0, 0);
        return str;
    }

    friend std::string shadowDllNameA();
};
#endif // lib.


#ifdef _UNICODE
# define ShadowCopyHelper ShadowCopyHelperW
# define shadowDllName shadowDllNameW
#else
# define ShadowCopyHelper ShadowCopyHelperA
# define shadowDllName shadowDllNameA
#endif

namespace vss {

using namespace cclib;

typedef std::map<std::string, shared_ptr<ShadowCopyHelperA>> vol_vss_map;

struct copyer
{
    copyer() { COMInitialize(); }

    //!
    /// brief: if error return empty std::string.
    ///        else return vss copy file name.
    std::string copy(std::string const &file)
    {
        std::string vol = win32::root(file);
        vol = to_lower(vol);

        shared_ptr<ShadowCopyHelperA> copyer;

        vol_vss_map::iterator it = vol_map_.find(vol);
        if (it == vol_map_.end()) {
            copyer.reset(new ShadowCopyHelperA(vol + win32::kseparator, shadowDllNameA()));
            vol_map_.insert(vol_vss_map::value_type(vol, copyer));
        } else {
            copyer = it->second;
        }
      
        if (!copyer->ready())
            return std::string();

        std::string vssfile;
        vssfile += copyer->shadowPath();
        vssfile += file.substr(2);

        return vssfile;
    }
private:
    vol_vss_map vol_map_;
};

inline copyer& makecopyer()
{
    return singleton<copyer>::ref();
}

} // namespace helper.

#endif // SHADOW_HELPER_H_