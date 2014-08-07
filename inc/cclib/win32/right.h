//!
/// brief: right.h define right about file and dir.
///

#if !defined(cclib_win32_right_h_)
#define cclib_win32_right_h_

#include <string>
#include "../types.h"
#include "../strutil.h"
#include "executer.h"

CCLIB_NAMESPACE_BEGIN
WIN32_NAMESPACE_BEGIN

#define SPECIAL_PATH_NUM 5
const std::string kspecialpath[SPECIAL_PATH_NUM] = {
    "C:\\Windows\\winsxs",
    "C:\\Windows\\winsxs\\Catalogs",
    "C:\\Windows\\winsxs\\Manifests",
    "C:\\Windows\\System32",
    "C:\\Windows\\SysWow64",
};

//!
/// brief: adjust path is a special path
///        a special path has a trustedinstaller right
///        so you must remove trustedinstaller right.
inline bool is_special(std::string const& path)
{
    for (int i = 0; i < SPECIAL_PATH_NUM; ++i) {
        if (start_with(path, kspecialpath[i], false))
            return true;
    }
    return false;
}

//!
/// brief: remove path trustedinstaller right
inline bool rm_trustedinstaller(std::string const& path)
{
    std::string chown_cmd;
    chown_cmd += "\"takeown.exe\" /F \"";
    chown_cmd += path;
    chown_cmd += "\"";

    uint32_t retcode;
    if (!execute(&retcode, "", chown_cmd) || retcode != 0)
        return false;

    std::string icacls_cmd;
    icacls_cmd += "\"icacls.exe\" \"";
    icacls_cmd += path;
    icacls_cmd += "\" /grant Administrators:F";

    if (!execute(&retcode, "", icacls_cmd) || retcode != 0)
        return false;

    return true;
}

CCLIB_NAMESPACE_END
WIN32_NAMESPACE_END

#endif // cclib_win32_right_h_