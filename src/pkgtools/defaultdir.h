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

#include <string>

namespace path {

    const std::string kRoot     = "%systemdrive%";
    const std::string kWindows  = "%windir%";
    const std::string kSystem   = "%windir%\\system32";
    const std::string kSysWow   = "%windir%\\syswow64";
    const std::string kSetup    =  "%programfiles%";
    const std::string kSetupWow = "%programfiles(x86)%";
    const std::string kUser     = "%username%";
    const std::string kUserProf = "%userprofile%";
    //TODO:: desktop dir???
    const std::string kDesktop  = "";
    const std::string kAppData  = "%appdata%";
    const std::string kLocalAppData = "%localappdata%";
    
    // for inner 
    namespace inner {
#define k(x) k##x="$"#x
    const std::string k(ROOT);
    const std::string k(WINDOWS);
    const std::string k(SYSTEM);
    const std::string k(SYSWOW);
    const std::string k(SETUP);
    const std::string k(SETUPWOW);
    const std::string k(USER);
    const std::string k(USERPROF);
    const std::string k(DESKTOP);
    const std::string k(APPDATA);
    const std::string k(LOCALAPPDATA);
#undef k
    } // namespace inner

    

    /*
    m_ShellConstants.add(_T("WINDIR"),CSIDL_WINDOWS,CSIDL_WINDOWS);
    m_ShellConstants.add(_T("SYSDIR"),CSIDL_SYSTEM,CSIDL_SYSTEM);
    m_ShellConstants.add(_T("QUICKLAUNCH"), CSIDL_APPDATA, CSIDL_APPDATA);
    m_ShellConstants.add(_T("SENDTO"),CSIDL_SENDTO, CSIDL_SENDTO);
    m_ShellConstants.add(_T("RECENT"),CSIDL_RECENT, CSIDL_RECENT);
    m_ShellConstants.add(_T("NETHOOD"), CSIDL_NETHOOD, CSIDL_NETHOOD);
    m_ShellConstants.add(_T("FONTS"), CSIDL_FONTS, CSIDL_FONTS);
    m_ShellConstants.add(_T("LOCALAPPDATA"), CSIDL_LOCAL_APPDATA, CSIDL_LOCAL_APPDATA);
    m_ShellConstants.add(_T("PRINTHOOD"), CSIDL_PRINTHOOD, CSIDL_PRINTHOOD);
    m_ShellConstants.add(_T("COOKIES"), CSIDL_COOKIES, CSIDL_COOKIES);
    m_ShellConstants.add(_T("HISTORY"), CSIDL_HISTORY, CSIDL_HISTORY);
    m_ShellConstants.add(_T("PROFILE"), CSIDL_PROFILE, CSIDL_PROFILE);
    m_ShellConstants.add(_T("RESOURCES"), CSIDL_RESOURCES, CSIDL_RESOURCES);
    m_ShellConstants.add(_T("RESOURCES_LOCALIZED"), CSIDL_RESOURCES_LOCALIZED, CSIDL_RESOURCES_LOCALIZED);
    m_ShellConstants.add(_T("CDBURN_AREA"), CSIDL_CDBURN_AREA, CSIDL_CDBURN_AREA);
    */

    /*
    m_ShellConstants.add(_T("SMPROGRAMS"),CSIDL_PROGRAMS, CSIDL_COMMON_PROGRAMS);
    m_ShellConstants.add(_T("SMSTARTUP"),CSIDL_STARTUP, CSIDL_COMMON_STARTUP);
    m_ShellConstants.add(_T("DESKTOP"),CSIDL_DESKTOPDIRECTORY, CSIDL_COMMON_DESKTOPDIRECTORY);
    m_ShellConstants.add(_T("STARTMENU"),CSIDL_STARTMENU, CSIDL_COMMON_STARTMENU);
    m_ShellConstants.add(_T("DOCUMENTS"),CSIDL_PERSONAL, CSIDL_COMMON_DOCUMENTS);
    
    m_ShellConstants.add(_T("FAVORITES"),CSIDL_FAVORITES, CSIDL_COMMON_FAVORITES);
    m_ShellConstants.add(_T("MUSIC"),CSIDL_MYMUSIC, CSIDL_COMMON_MUSIC);
    m_ShellConstants.add(_T("PICTURES"),CSIDL_MYPICTURES, CSIDL_COMMON_PICTURES);
    m_ShellConstants.add(_T("VIDEOS"),CSIDL_MYVIDEO, CSIDL_COMMON_VIDEO);
    
    m_ShellConstants.add(_T("TEMPLATES"), CSIDL_TEMPLATES, CSIDL_COMMON_TEMPLATES);
    m_ShellConstants.add(_T("APPDATA"), CSIDL_APPDATA, CSIDL_COMMON_APPDATA);
    
    //m_ShellConstants.add(_T("ALTSTARTUP"), CSIDL_ALTSTARTUP, CSIDL_COMMON_ALTSTARTUP);
    m_ShellConstants.add(_T("INTERNET_CACHE"), CSIDL_INTERNET_CACHE, CSIDL_INTERNET_CACHE);
    
    m_ShellConstants.add(_T("ADMINTOOLS"), CSIDL_ADMINTOOLS, CSIDL_COMMON_ADMINTOOLS);
    
    */

} // namespace path

#endif // pkgtools_default_dir_h_