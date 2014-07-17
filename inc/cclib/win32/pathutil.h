//!
/// brief: pathutil.h define path utility.
///

#if !defined(cclib_win32_path_util_h_)
#define cclib_win32_path_util_h_

#include <windows.h>
#include <io.h>
#include <direct.h>
#include "../types.h"
#include "../strutil.h"

#ifndef MAX_PATH
#define MAX_PATH 260
#endif

CCLIB_NAMESPACE_BEGIN
WIN32_NAMESPACE_BEGIN

const char kchcurrpath = '.';
const std::string kregseparator = "\\/";
const char kchseparator = '\\';
const char kchlinuxsep = '/';
const std::string kseparator = "\\";
const std::string klinuxsep = "/";
const int kmaxpath = MAX_PATH;

//!
/// brief: path is existed, path include dir or file.
inline bool existed(std::string const& path)
{
    return _access(path.c_str(), 0) == 0;
}

//!
/// brief: is_dir adjust path is dir.
inline bool is_dir(std::string const& path)
{
    return !!(GetFileAttributesA(path.c_str()) & FILE_ATTRIBUTE_DIRECTORY);
}

inline std::string curr_dir()
{
    char dir[MAX_PATH] = {0};
    return _getcwd(dir, MAX_PATH);
}

//!
/// brief: like *nix os have root path, windows root path is volume path
///        likely "c:";
inline std::string root(std::string const& path = ".")
{
    if (path.empty() || path[0] == kchcurrpath) {
        std::string dir = curr_dir();
        return dir.substr(0, 2);
    }
    return path.substr(0, 2);
}

inline std::string& add_sep(std::string& path)
{
    std::string::size_type pos;

    pos = path.find_last_of(kregseparator);
    if (pos != path.length() - 1) {
        path += kseparator;
        return path;
    }
    return path;
}

//!
/// brief: pdir get path parent dir
/// BUG: if you path is likely "C:\\", then return what???
///      
inline std::string pdir(std::string const& path)
{
    /// absoluted path.
    std::string tmp = path;
    std::string::size_type index = path.length() - 1;
    if (path[index] == kchseparator || path[index] == kchlinuxsep)
        tmp = path.substr(0, index - 1);

    index = tmp.find_last_of(kregseparator);
    if (index == std::string::npos)
        return tmp;

    return tmp.substr(0, index);
}

//!
/// brief: mkdir make dir.
inline bool mkdir(std::string const& dir)
{
    return _mkdir(dir.c_str()) == 0;
}

//!
/// brief: mkdirtree to make dir resurive.
inline bool mkdirtree(std::string const& dir)
{
    std::vector<std::string> out;
    split(dir, kseparator, &out);

    std::string dirs = "";
    for (size_t i = 0; i < out.size(); ++i) {
        dirs += out[i];
        dirs += kseparator;
        if (!existed(dirs))
            if (!mkdir(dirs))
                return false;     
    }
    return true;
}

CCLIB_NAMESPACE_END
WIN32_NAMESPACE_END

#endif // cclib_win32_path_util_h_