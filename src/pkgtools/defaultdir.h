//!
/// brief: defaultdir.h define some default dir
///
/// example:  <win vista or later>
///         %ROOT%      "C:", "D:";
///         %WIN%       "C:\\Windows"
/// WARNING: disable filesystem redirection.
///         %SYSTEM%    "C:\\Windows\\System32" 64bit programm in 64bit os.
///                     "C:\\Windows\\System32" 32bit programm in 32bit os.
///         %SYSWOW%    "C:\\Windows\\SysWow64" 32bit programm in 64bit os.
///         %SETUP%     "C:\\Program Files"      64bit programm in 64bit os.
///                     "C:\\Program Files"      32bit programm in 32bit os.
///         %SETUPWOW%  "C:\\Program Files (x86)" 32bit programm in 64bit os.         
///                     
///         %USER%      "yxy"
///         %PROF%      "C:\\User\\yxy"
///         %LAPPDATA%  "C:\\Users\\yxy\\AppData\\Roaming"
///         %APPDATA%   "C:\\Users\\yxy\\AppData\\Local"
///

#if !defined(pkgtools_default_dir_h_)
#define pkgtools_default_dir_h_

#include <shlobj.h>
#include <string>
#include <map>
#include <cclib/types.h>

namespace path {
    const cclib::uint32_t kShiftBitsWithCommon = 16;
    const cclib::uint32_t kLow16BitsMask = 0x0000ffff;
    const cclib::uint32_t kHigh16BitsMask = 0xffff0000;
    typedef std::map<std::string, cclib::uint32_t> spec_os_folder_t;

    inline cclib::uint32_t makeIndex(cclib::uint32_t low, cclib::uint32_t high = 0)
    {
        return low | (high << kShiftBitsWithCommon);
    }

    inline cclib::uint32_t splitLow(cclib::uint32_t index)
    {
        return index & kLow16BitsMask;
    }

    inline cclib::uint32_t splitHigh(cclib::uint32_t index)
    {
        return (index & kHigh16BitsMask) >> kShiftBitsWithCommon;
    }

    inline size_t makeSpecOSFolder(spec_os_folder_t *folder)
    {
        size_t orisz = folder->size();
        folder->insert(std::make_pair("WINDIR",  CSIDL_WINDOWS));
        folder->insert(std::make_pair("SYSDIR",  CSIDL_SYSTEM));
        folder->insert(std::make_pair("SENDTO",  CSIDL_SENDTO));
        folder->insert(std::make_pair("RECENT",  CSIDL_RECENT));
        folder->insert(std::make_pair("NETHOOD", CSIDL_NETHOOD));
        folder->insert(std::make_pair("FONTS",   CSIDL_FONTS));        
        folder->insert(std::make_pair("COOKIES", CSIDL_COOKIES));
        folder->insert(std::make_pair("HISTORY", CSIDL_HISTORY));
        folder->insert(std::make_pair("PROFILE", CSIDL_PROFILE));
        folder->insert(std::make_pair("PRINTHOOD",    CSIDL_PRINTHOOD));
        folder->insert(std::make_pair("RESOURCES",    CSIDL_RESOURCES));
        folder->insert(std::make_pair("CDBURN_AREA",  CSIDL_CDBURN_AREA));
        folder->insert(std::make_pair("QUICKLAUNCH",  CSIDL_APPDATA));
        folder->insert(std::make_pair("LOCALAPPDATA", CSIDL_LOCAL_APPDATA));
        folder->insert(std::make_pair("RESOURCES_LOCALIZED", CSIDL_RESOURCES_LOCALIZED));
        // if can't find current user path, then need use common user path.
        folder->insert(std::make_pair("SMPROGRAMS", makeIndex(CSIDL_PROGRAMS, CSIDL_COMMON_PROGRAMS)));
        folder->insert(std::make_pair("SMSTARTUP",  makeIndex(CSIDL_STARTUP, CSIDL_COMMON_STARTUP)));
        folder->insert(std::make_pair("DESKTOP",    makeIndex(CSIDL_DESKTOPDIRECTORY, CSIDL_COMMON_DESKTOPDIRECTORY)));
        folder->insert(std::make_pair("STARTMENU",  makeIndex(CSIDL_STARTMENU, CSIDL_COMMON_STARTMENU)));
        folder->insert(std::make_pair("DOCUMENTS",  makeIndex(CSIDL_PERSONAL, CSIDL_COMMON_DOCUMENTS)));   
        folder->insert(std::make_pair("FAVORITES",  makeIndex(CSIDL_FAVORITES, CSIDL_COMMON_FAVORITES)));
        folder->insert(std::make_pair("MUSIC",      makeIndex(CSIDL_MYMUSIC, CSIDL_COMMON_MUSIC)));
        folder->insert(std::make_pair("PICTURES",   makeIndex(CSIDL_MYPICTURES, CSIDL_COMMON_PICTURES)));
        folder->insert(std::make_pair("VIDEOS",     makeIndex(CSIDL_MYVIDEO, CSIDL_COMMON_VIDEO)));
        folder->insert(std::make_pair("TEMPLATES",  makeIndex(CSIDL_TEMPLATES, CSIDL_COMMON_TEMPLATES)));
        folder->insert(std::make_pair("APPDATA",    makeIndex(CSIDL_APPDATA, CSIDL_COMMON_APPDATA)));
        folder->insert(std::make_pair("INTERNET_CACHE", makeIndex(CSIDL_INTERNET_CACHE, CSIDL_INTERNET_CACHE)));
        folder->insert(std::make_pair("ADMINTOOLS",     makeIndex(CSIDL_ADMINTOOLS, CSIDL_COMMON_ADMINTOOLS));
        
        return (size_t)(folder->size() - orisz);
    }

} // namespace path

#endif // pkgtools_default_dir_h_