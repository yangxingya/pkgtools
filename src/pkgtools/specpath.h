//!
/// brief: specpath.h define some special path
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

#if !defined(pkgtools_spec_path_h_)
#define pkgtools_spec_path_h_

#include <shlobj.h>
#include <string>
#include <map>
#include <cclib/types.h>

namespace path {
using namespace cclib;

const uint16_t kShiftBitsWithCommon = 8;
const uint16_t kLow8BitsMask  = 0x00ff;
const uint16_t kHigh8BitsMask = 0xff00;

typedef std::map<std::string, uint16_t> SpecPath2CsidlMap;

inline uint16_t makeIndex(uint16_t low, uint16_t high = 0)
{
    return low | (high << kShiftBitsWithCommon);
}

inline uint16_t splitLow(uint16_t index)
{
    return index & kLow8BitsMask;
}

inline uint16_t splitHigh(uint16_t index)
{
    return (index & kHigh8BitsMask) >> kShiftBitsWithCommon;
}

const uint16_t kProgFiles     = 0x00ff;
const uint16_t kProgFilesComm = 0x00fe;

inline size_t makeSpec(SpecPath2CsidlMap *sp2csidl)
{
    size_t orisz = sp2csidl->size();
    sp2csidl->insert(std::make_pair("WINDIR",  CSIDL_WINDOWS));
    sp2csidl->insert(std::make_pair("SYSDIR",  CSIDL_SYSTEM));
    sp2csidl->insert(std::make_pair("SENDTO",  CSIDL_SENDTO));
    sp2csidl->insert(std::make_pair("RECENT",  CSIDL_RECENT));
    sp2csidl->insert(std::make_pair("NETHOOD", CSIDL_NETHOOD));
    sp2csidl->insert(std::make_pair("FONTS",   CSIDL_FONTS));        
    sp2csidl->insert(std::make_pair("COOKIES", CSIDL_COOKIES));
    sp2csidl->insert(std::make_pair("HISTORY", CSIDL_HISTORY));
    sp2csidl->insert(std::make_pair("PROFILE", CSIDL_PROFILE));
    sp2csidl->insert(std::make_pair("PRINTHOOD",    CSIDL_PRINTHOOD));
    sp2csidl->insert(std::make_pair("RESOURCES",    CSIDL_RESOURCES));
    sp2csidl->insert(std::make_pair("CDBURN_AREA",  CSIDL_CDBURN_AREA));
    sp2csidl->insert(std::make_pair("QUICKLAUNCH",  CSIDL_APPDATA));
    sp2csidl->insert(std::make_pair("LOCALAPPDATA", CSIDL_LOCAL_APPDATA));
    sp2csidl->insert(std::make_pair("RESOURCES_LOCALIZED", CSIDL_RESOURCES_LOCALIZED));
    // if can't find current user path, then need use common user path.
    sp2csidl->insert(std::make_pair("SMPROGRAMS", makeIndex(CSIDL_PROGRAMS, CSIDL_COMMON_PROGRAMS)));
    sp2csidl->insert(std::make_pair("SMSTARTUP",  makeIndex(CSIDL_STARTUP, CSIDL_COMMON_STARTUP)));
    sp2csidl->insert(std::make_pair("DESKTOP",    makeIndex(CSIDL_DESKTOPDIRECTORY, CSIDL_COMMON_DESKTOPDIRECTORY)));
    sp2csidl->insert(std::make_pair("STARTMENU",  makeIndex(CSIDL_STARTMENU, CSIDL_COMMON_STARTMENU)));
    sp2csidl->insert(std::make_pair("DOCUMENTS",  makeIndex(CSIDL_PERSONAL, CSIDL_COMMON_DOCUMENTS)));   
    sp2csidl->insert(std::make_pair("FAVORITES",  makeIndex(CSIDL_FAVORITES, CSIDL_COMMON_FAVORITES)));
    sp2csidl->insert(std::make_pair("MUSIC",      makeIndex(CSIDL_MYMUSIC, CSIDL_COMMON_MUSIC)));
    sp2csidl->insert(std::make_pair("PICTURES",   makeIndex(CSIDL_MYPICTURES, CSIDL_COMMON_PICTURES)));
    sp2csidl->insert(std::make_pair("VIDEOS",     makeIndex(CSIDL_MYVIDEO, CSIDL_COMMON_VIDEO)));
    sp2csidl->insert(std::make_pair("TEMPLATES",  makeIndex(CSIDL_TEMPLATES, CSIDL_COMMON_TEMPLATES)));
    sp2csidl->insert(std::make_pair("APPDATA",    makeIndex(CSIDL_APPDATA, CSIDL_COMMON_APPDATA)));
    sp2csidl->insert(std::make_pair("INTERNET_CACHE", makeIndex(CSIDL_INTERNET_CACHE, CSIDL_INTERNET_CACHE)));
    sp2csidl->insert(std::make_pair("ADMINTOOLS",     makeIndex(CSIDL_ADMINTOOLS, CSIDL_COMMON_ADMINTOOLS)));
    
    // custom define: dont related users and special user, is global.
    // but has x86 or x64 dir in 64bit os.
    // LIMITS:
    //      1. if app is 32bits, then the installed file in 'C:\\Program Files (x86)' dir
    //      2. if app is 64bits, then the installed file in 'C:\\Program Files' dir
    // DETAIL:
    //      1.if you app is 32bits then you can only access to 'C:\\Program Files (x86)' dir
    //        when you use 'C:\\Program Files' dir. but you can 
    //        DISABLE FILESYSTEM REDIRECTION, then 'C:\\Program Files' is 
    //        'C:\\Program Files' dir not x86's.

    sp2csidl->insert(std::make_pair("PROGFILES",    CSIDL_PROGRAM_FILES));
    sp2csidl->insert(std::make_pair("PROGFILESX86", CSIDL_PROGRAM_FILESX86));
    sp2csidl->insert(std::make_pair("PROGFILES_COMM",    CSIDL_PROGRAM_FILES_COMMON));
    sp2csidl->insert(std::make_pair("PROGFILESX86_COMM", CSIDL_PROGRAM_FILES_COMMONX86));

    return (size_t)(sp2csidl->size() - orisz);
}

} // namespace path

#endif // pkgtools_spec_path_h_