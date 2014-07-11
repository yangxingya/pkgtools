//!
/// brief: pathconv.h define path convertor 
///

#if !defined(pkgtools_path_conv_h_)
#define pkgtools_path_conv_h_

#include <sstream>
#include <cclib/types.h>
#include <cclib/strutil.h>
#include "specpath.h"
#include "error.h"
#include "except.h"

#ifndef MAX_PATH
#define MAX_PATH 260
#endif

namespace path {

using namespace cclib;

const std::string kcustompathpfx = "$";

struct miniPath 
{
    /// is special path?
    bool special;
    /// if it is special path, its CSIDL_xxx value.
    uint16_t csidl;
    /// prefix, csidl value:
    ///   if path = "C:\\Program Files\\test.txt", then
    ///   prefix is: "C:\\Program Files"
    /// if csidl split to high and low section and high
    /// section is not zero, then have pfx path related
    /// <All Users>, pfx path is related <Current User>
    std::string pfx;
    /// std::string pfx1;
    /// suffix, 
    ///   if it is special path = "C:\\Program Files\\test.txt",
    ///       then suffix is: "test.txt"
    ///   else
    ///       suffix is whole path.
    std::string sfx;
    /// pkgpath:
    ///   package path: "$<uint16_t>\\suffix" or "C:\\dir\\test.txt";
    std::string pkg;

    minPath(std::string const& path)
    {
        if (findCustom(path)) return;
        
    }
private:
    /// find likely "$WINDIR\\xxx.txt" custom format.
    bool findCustom(std::string const& path)
    {
        SpecPath2CsidlMap &spec2csidl = customMap();
     
        char folder[MAX_PATH];
        std::string start;
        SpecPath2CsidlMap::iterator it = spec2csidl.begin();
        for (; it != spec2csidl.end(); ++it) {
            start = kcustompathpfx + it->first;
            if (start_with(path, start) {
                csidl = it->second;
                int low = splitLow(csidl);
                int high = splitHigh(csidl);
                /// get special folder for current user or global.
                if (!SHGetSpecialFolderPathA(0, folder, low, 0)) {
                    /// if no high, it is a global special path,
                    /// but get error, throw except.
                    if (high == 0)
                        throwSpecGetExcept(start, low);

                    /// if has high part, 'Current User' get failed,
                    /// then need get 'All Users' path.
                    if (!SHGetSpecialFolderPathA(0, folder, high, 0)) 
                        throwSpecGetExcept(start, high);

                    ///pfx1 = folder;
                }

                special = true;

                pfx = folder;
                sfx = path.substr(start.length() + 2);
                
                std::stringstream ss;
                std::string to;

                /// make pkg path is likely:
                ///   "$45\\text.txt" format.
                ss << it->second;
                ss >> to;
                pkg = path;
                replace(pkg, it->first, to);
                return true;
            }
        }
        return false;   
    }
    
    bool findCommon(std::string const& path)
    {
        //HACK: if you can improve it, please email:xingyayang@163.com
        specil = false;
        pfx = "";
        sfx = path;
        pkg = path;
        
        return false;
    }

    void throwSpecGetExcept(std::string const& start, int csidl)
    {
        std::stringstream ess;
        ess << "Get special folder path failed! prefix: "
            << start << ", CSIDL_xxx: 0x" 
            << std::hex() << csidl;
        std::string error;
        ess >> error;
        LOG(ERROR) << error;
        throw spget_error(ERROR_SpecFolderGetError, error);
    }
    static SpecPath2CsidlMap& customMap()
    {
        static SpecPath2CsidlMap spec2csidl;
        
        if (spec2csidl.size() == 0)
            makeSpec(&spec2csidl);

        return spec2csidl;
    }
};

struct PathConvertor
{
    static bool getMiniPath(std::string const& path, miniPath
};

} // namespace path

#endif // pkgtools_path_conv_h_