//!
/// brief: pathconv.h define path convertor 
///

#if !defined(pkgtools_argv_trans_h_)
#define pkgtools_argv_trans_h_

#include <sstream>
#include <cclib/types.h>
#include <cclib/strutil.h>
#include <cclib/win32/pathutil.h>
#include "specpath.h"
#include "argv.h"
#include "error.h"
#include "except.h"

#ifndef MAX_PATH
#define MAX_PATH 260
#endif

namespace argv {

using namespace cclib;
using namespace path;

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
    std::string pkgargv;

    miniPath(std::string const& path)
    {
        if (findCustom(path)) return;
        if (findCommon(path)) return;

        noTransfer(path);     
    }
private:
    /// find likely "$WINDIR\\xxx.txt" custom format.
    bool findCustom(std::string const& path)
    {
        /// if path is not likely "$xxx" format, then
        /// the format never custom, because custom format
        /// is: $xxx, must '$' with started.
        if (!start_with(path, kcustompathpfx)) 
            return false;

        SpecPath2CsidlMap &spec2csidl = customMap();
        char folder[MAX_PATH];
        std::string start;
        SpecPath2CsidlMap::iterator it = spec2csidl.begin();

        for (; it != spec2csidl.end(); ++it) {
            start = kcustompathpfx + it->first;
            if (!start_with(path, start)) 
                continue;

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
            pkgargv = path;
            replace(pkgargv, it->first, to);
            return true; 
        }
        return false;   
    }

    bool findCommon(std::string const& path)
    {
        std::map<uint16_t, std::string> &paths = csidlPaths();
        std::string lpath = to_lower(path);
        std::map<uint16_t, std::string>::iterator it;

        for (it = paths.begin(); it != paths.end(); ++it) {
           
            if (!start_with(lpath, it->second)) 
                continue;
            
            special = true;

            pfx = path.substr(0, it->second.length());
            sfx = path.substr(it->second.length() + 2);

            std::stringstream ss;
            std::string to;

            /// make pkg path is likely:
            ///   "$45\\text.txt" format.
            ss << kcustompathpfx << it->first;
            ss >> to;
            pkgargv = path;
            replace(pkgargv, it->second, to);
            return true;       
        }
        return false;
    }

    bool noTransfer(std::string const& path)
    {
        special = false;
        pfx = "";
        sfx = path;
        pkgargv = path;

        return true;
    }

    void throwSpecGetExcept(std::string const& start, int csidl)
    {
        std::stringstream ess;
        ess << "Get special folder path failed! prefix: "
            << start << ", CSIDL_xxx: 0x" 
            << std::hex << csidl;
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
    static std::map<uint16_t, std::string>& csidlPaths()
    {
        static std::map<uint16_t, std::string> paths;

        if (paths.size() == 0) {
            SpecPath2CsidlMap &maps = customMap();
            
            char folder[MAX_PATH];
            SpecPath2CsidlMap::iterator it;

            for (it = maps.begin(); it != maps.end(); ++it) {
                uint16_t csidl = it->second;
                uint16_t low;
                uint16_t high;

                low = splitLow(csidl);

                if (SHGetSpecialFolderPathA(0, folder, low, 0))
                    paths.insert(std::make_pair(low, to_lower(folder)));

                if ((high = splitHigh(csidl)) == 0) continue;

                if (SHGetSpecialFolderPathA(0, folder, high, 0))
                    paths.insert(std::make_pair(high, to_lower(folder))); 
            }
        }
        return paths;
    }
};

} // namespace path

namespace entry {

using namespace path;
using namespace argv;

struct transfer
{
    transfer(std::vector<std::string> &argvs)
        : argvs_(argvs) {}
    void operator()(AutoArgv argv)
    {
        std::string str = "";
        switch (argv->type()) {
        case entry::kFile:
            str = fileDst(argv);
            break;
        case entry::kDir:
            str = dirDst(argv);
            break;
        case entry::kExec:
            str = execCmd(argv);
            break;
        case entry::kOut:
            // null.
            break;
        case entry::kUnknown:
            // throw exception/
            break;
        }
        if (!str.empty())
            argvs_.push_back(str);
    }
private:
    std::vector<std::string> &argvs_;
    std::string fileDst(AutoArgv argv)
    {
        FileArgv *fargv = (FileArgv *)argv.get();
        miniPath minipath(fargv->dst());
        return minipath.pkgargv;
    }
    std::string dirDst(AutoArgv argv)
    {
        DirArgv *dargv = (DirArgv *)argv.get();
        miniPath minipath(dargv->dst());
        return minipath.pkgargv;
    }
    std::string execCmd(AutoArgv argv)
    {
        ExecArgv *eargv = (ExecArgv *)argv.get();
        return eargv->cmd();
    }
};

struct restorer 
{
    restorer(
        std::vector<std::string> const& orig_args, AutoArgvList &arglist
        , bool exorinst = false, std::string const& todir = "")
            : orig_args_(orig_args), arglist_(arglist)
            , exorinst_(exorinst), todir_(todir) 
    {
    
    }
    
    void operator()(entry_t entry)
    {
        std::string argv = orig_args_[(uint32_t)entry.strindex];

        /// if type is not file or dir then dont change it.
        if (entry.type != kentryfile && entry.type != kentrydir) {
            out_args_.push_back(argv);
            return;
        }

        /// type is file or dir.

        /// start not $
        if (!start_with(argv, kcustompathpfx)) {
            std::string path = "";

            /// if extract then need change 'C:\\xxx' to 'C$\\xxx' format.
            if (exorinst_) {
                path = todir_;
                win32::add_sep(path);

                /// change 'C:\\xxx' to 'C$\\xxx' format.
                argv[1] = kcustompathpfx[0];
            }

            path += argv;
            out_args_.push_back(path);
            return;
        }

        /// start with $

        /// extract
        if (exorinst_) {
            std::string path = todir_;

            win32::add_sep(path);

            ///todo:: change to $WINDIR format.
            path += argv;
            out_args_.push_back(path);

            return;
        }

        /// install

        std::string::size_type pos = argv.find_first_of(win32::kregseparator);
        std::string pfx = argv.substr(0, pos + 1); 
        std::string strcsidl = pfx.substr(1);

        uint16_t csidl;
        std::stringstream ss;
        ss << strcsidl;
        ss >> csidl;

        std::string path = customPath(csidl);
        if (path.empty()) {
            std::stringstream ss;
            ss << "Custom path is empty! CSIDL: 0x" << std::hex << csidl;
            std::string error;

            ss >> error;
            LOG(ERROR) << error;
            throw spget_error(ERROR_CustomPathGetFailed, error);
        }

        std::string tmp = argv;
        replace(tmp, pfx, path);

        DLOG(INFO) << "restorer, custom path: " << path;
        DLOG(INFO) << "restorer, custom replace path: " << tmp;

        out_args_.push_back(tmp); 
    }
private:
    std::vector<std::string> const& orig_args_;
    std::vector<std::string> out_args_;
    AutoArgvList &arglist_;
    bool exorinst_;
    std::string todir_;
};

} // namespace argv

#endif // pkgtools_argv_trans_h_