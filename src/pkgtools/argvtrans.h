//!
/// brief: pathconv.h define path convertor 
///

#if !defined(pkgtools_argv_trans_h_)
#define pkgtools_argv_trans_h_

#include <sstream>
#include <cclib/types.h>
#include <cclib/strutil.h>
#include <cclib/win32/pathutil.h>
#include "except.h"
#include "specpath.h"
#include "argv.h"
#include "counter.h"

namespace argv {

using namespace cclib;
using namespace path;
using namespace err;

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
            sfx = "";
            if (path.length() > start.length() + 1)
                sfx = path.substr(start.length() + 1);

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
using namespace cclib;
using namespace pkg;

struct transfer
{
    transfer(acounter &counter, std::vector<uint64_t> &str_index)
        : counter_(counter), str_index_(str_index) 
    {}
    void operator()(AutoArgv argv)
    {
        std::string str = "";
        switch (argv->type()) {
        case kAddf:    str = addfDst(argv); break;
        case kDelf:    str = delfDst(argv); break;
        case kMkdir:   str = mkdirDst(argv);  break;
        case kRmdir:   str = rmdirDst(argv); break;
        case kExec:    str = execCmd(argv); break;
        case kSetting: str = setFlags(argv); break;
        case kOut:     break;
        case kUnknown: break;
        }

        if (str.empty()) {
            str_index_.push_back(kinvalid);
            return;
        }
        str_index_.push_back(counter_.index(str));
    }
private:
    std::vector<uint64_t> &str_index_;
    acounter &counter_;

    std::string addfDst(AutoArgv argv)
    {
        AddfArgv *fargv = (AddfArgv *)argv.get();
        miniPath minipath(fargv->dst());
        return minipath.pkgargv;
    }

    std::string delfDst(AutoArgv argv)
    {
        DelfArgv *fargv = (DelfArgv *)argv.get();
        miniPath minipath(fargv->dst());
        return minipath.pkgargv;
    }

    std::string mkdirDst(AutoArgv argv)
    {
        MkdirArgv *dargv = (MkdirArgv *)argv.get();
        miniPath minipath(dargv->dst());
        return minipath.pkgargv;
    }

    std::string rmdirDst(AutoArgv argv)
    {
        RmdirArgv *dargv = (RmdirArgv *)argv.get();
        miniPath minipath(dargv->dst());
        return minipath.pkgargv;
    }

    std::string execCmd(AutoArgv argv)
    {
        ExecArgv *eargv = (ExecArgv *)argv.get();
        std::string cmd = eargv->cmd();
    
        std::vector<std::string> out;
        split(cmd, kcustompathpfx, win32::kseparator, &out);

        std::string tmp;
        for (size_t i = 0; i < out.size(); ++i) {
            tmp = kcustompathpfx + out[i];
            miniPath path(tmp);
            replace(cmd, tmp, path.pkgargv);   
        }

        return cmd;
    }

    std::string setFlags(AutoArgv argv)
    {
        SettingArgv *sargv = (SettingArgv *)argv.get();
        return sargv->flags();
    }
};

struct restorer 
{
    restorer(
        std::vector<std::string> const& args, AutoArgvList &arglist
        , bool exorinst = false, std::string const& todir = "")
            : args_(args), arglist_(arglist)
            , exorinst_(exorinst), todir_(todir) 
    {
    
    }
    
    void operator()(entry_t entry)
    {
        std::string args = args_[(uint32_t)entry.strindex];
        AutoArgv arg;

        switch (entry.type) {
        case kentryaddf:
            arg.reset(new AddfArgv(entry, transfer(args), entry.dtaindex));
            break;
        case kentrydelf: 
            arg.reset(new DelfArgv(entry, transfer(args)));
            break;
        case kentrymkdir:
            arg.reset(new MkdirArgv(entry, transfer(args)));
            break;
        case kentryrmdir:
            arg.reset(new RmdirArgv(entry, transfer(args)));
            break;
        case kentryexec:
            arg.reset(new ExecArgv(entry, transfer2(args)));
            break;
        case kentrysetting:
            arg.reset(new SettingArgv(entry, args));
            break;
        default:
            {
                std::stringstream ss;
                ss << "restorer: not support type: " << entry.type;
                std::string error;
                ss >> error;
                LOG(ERROR) << error;
                throw pkg_error(ERROR_EntryTypeNotSupported, error);
            }
        }
        
        arglist_.push_back(arg);

    }
private:
    std::vector<std::string> const& args_;
    AutoArgvList &arglist_;
    bool exorinst_;
    std::string todir_;

    std::string transfer(std::string const& str)
    {
        std::string argv = str;

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
            return path;
        }

        /// start with $

        /// extract
        if (exorinst_) {
            std::string path = todir_;

            win32::add_sep(path);

            ///todo:: change to $WINDIR format.
            path += argv;
            return path;
        }

        /// install

        std::string::size_type pos = argv.find_first_of(win32::kregseparator);
        std::string pfx = argv.substr(0, pos); 
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

        return tmp;
    }




    /// only for exec argv ""$xxx\\xxx.exe"" %guid   ";
    /// replace $xxx -> special path.
    std::string transfer2(std::string const& str)
    {
        std::string argv = str;

        /// type is file or dir.
#if 0

        /// start not $
        std::string::size_type pos;

        /// not find $xxx\\xxx\\yyy.exe.
        if ((pos = str.find(kcustompathpfx)) == std::string::npos)
            return str;


        /// find $xxx\\.... -> '\\' position.
        std::string::size_type pos2 = argv.find_first_of(win32::kregseparator);
        if ((pos2 = argv.find_first_of(win32::kregseparator))
            == std::string::npos) {
            std::stringstream ss;
            ss << "Custom path is error! :" << str;
            std::string error;

            ss >> error;
            LOG(ERROR) << error;
            throw spget_error(ERROR_CustomPathGetFailed, error);
        }

        std::string pfx = argv.substr(pos, pos2 - 1); 
        std::string strcsidl = pfx.substr(1);
#endif

        std::vector<std::string> out;
        split(str, kcustompathpfx, win32::kseparator, &out);

        std::string tmp;
        for (size_t i = 0; i < out.size(); ++i) {

            uint16_t csidl;
            std::stringstream ss;
            ss << out[i];
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
            
            tmp = kcustompathpfx + out[i];
            replace(argv, tmp, path);
        }

        DLOG(INFO) << "restorer, original path: " << str;
        DLOG(INFO) << "restorer, custom replace path: " << argv;

        return argv;
    }
};

} // namespace argv

#endif // pkgtools_argv_trans_h_