//!
/// brief: installer.h define install a package to pc.
///

#if !defined(pkgtools_installer_h_)
#define pkgtools_installer_h_

#include <string>
#include <cclib/types.h>
#include <cclib/strutil.h>
#include <cclib/win32/executer.h>
#include "unpacker.h"
#include "pkgfile.h"
#include "argv.h"
#include "argvdef.h"

namespace pkg {

using namespace cclib;
using namespace argv;

struct Installer
{
    Installer(std::string const& pkgfile, bool witharg, std::string const& arglist)
        : unpacker_(pkgfile), witharg_(witharg), arglist_(arglist) {}

    int install()
    {
        LOG(INFO) << "Install args: \"" << arglist_ << "\"";
        split(arglist_, " ", "=", &arg_map_);

        /// first: get unpacker entrys and arglist.
        std::vector<entry_t> const& entrys = unpacker_.entrys();
        std::vector<std::string> const& args = unpacker_.arglist();

        /// second: restore arglist to current dir.
        AutoArgvList arglist;
        entry::restorer restor(args, arglist);
        std::for_each(entrys.begin(), entrys.end(), restor);
    
        int ret;
        /// third: install every entry sequencely.
        for (size_t i = 0L; i < arglist.size(); ++i) {
            if ((ret = install(arglist[i])) != ERROR_Success)
                return ret;
        }
        return ERROR_Success;
    }

private:
    unpacker unpacker_;
    bool witharg_;
    std::string arglist_;
    std::map<std::string, std::string> arg_map_;

#pragma region entry install
    int install(AutoArgv argv)
    {
        switch (argv->type()) {
        case entry::kFile:    return entryInstall((FileArgv*   )argv.get());
        case entry::kDir:     return entryInstall((DirArgv*    )argv.get());
        case entry::kExec:    return entryInstall((ExecArgv*   )argv.get());
        case entry::kSetting: return entryInstall((SettingArgv*)argv.get());
        default: 
            LOG(ERROR) << "Install: Not support type: " << argv->type();
            return ERROR_EntryTypeNotSupported;
        }
    }

    int entryInstall(FileArgv *argv)
    {
        uint8_t flags = (uint8_t)argv->instFlags();

        if (!(flags & kinstmask)) {
            LOG(INFO) << "Install: file: \"" << argv->dst() << "\" no need install!";
            return ERROR_Success;
        }

        int ret;
        /// support install.
        try {
            unpacker_.tofile(argv->dst(), argv->offset());
            LOG(INFO) << "Install: file: \"" << argv->dst() << "\" RAW copy successful!";
            return ERROR_Success;
        } catch (except_base &ex) {
            ret = ex.error();
            LOG(WARNING) << "Install: file: \"" << argv->dst() << "\" RAW copy failed!!!";
        }

        /// error try
        if ((flags & kerrtmask) == kerrtry) {
            /// need remove file dir relatived right.<TrustedInstaller right>.

        }

        /// error ignore
        if ((flags & kerrxmask) == kerrignore) {
            LOG(WARNING) << "Install: file: \"" << argv->dst() << "\" copy failed, ignore error, continue...";
            return ERROR_Success;
        }

        LOG(ERROR) << "Install: file: \"" << argv->dst() << "\" copy failed!";
        return ret;
    }

    int entryInstall(DirArgv *argv)
    {
        uint8_t flags = (uint8_t)argv->instFlags();
        if (!(flags & kinstmask)) {
            LOG(INFO) << "Install: dir: \"" << argv->dst() << "\" no need install!";
            return ERROR_Success;
        }

        if (win32::mkdirtree(argv->dst())) {
            LOG(INFO) << "Install: dir: \"" << argv->dst() << "\" create successful!";
            return ERROR_Success;
        }
        
        /// error try
        if ((flags & kerrtmask) == kerrtry) {
            /// need remove parent dir relatived right.<TrustedInstaller right>.

        }

        /// error ignore
        if ((flags & kerrxmask) == kerrignore) {
            LOG(WARNING) << "Install: dir: \"" << argv->dst() << "\" create failed, ignore error, continue...";
            return ERROR_Success;
        }

        LOG(ERROR) << "Install: dir: \"" << argv->dst() << "\" create failed!";
        return ERROR_DirEntryInstFailed;
    }

    int entryInstall(ExecArgv *argv)
    {
        uint8_t flags = (uint8_t)argv->instFlags();
        if (!(flags & kinstmask)) {
            LOG(INFO) << "Install: exec: \"" << argv->cmd() << "\" no need install!";
            return ERROR_Success;
        }

        std::string cmd = cmd_replace(argv->cmd());
        
        uint32_t retcode;
        if (win32::execute(&retcode, "", cmd)) {
            LOG(INFO) << "Install: exec: \"" << argv->cmd() << "\" ==> \"" << cmd << "\" execute successful!";

            if (!argv->ckFlags()) {
                LOG(INFO) << "Install: exec: \"" << argv->cmd() << "\" ==> \"" << cmd << "\" no need check return code";
                return ERROR_Success;
            }
            
            /// need check return.
            if (argv->ckReturn() == retcode) {
                LOG(INFO) << "Install: exec: \"" << argv->cmd() << "\" ==> \"" << cmd << "\"  check return code successful!";
                return ERROR_Success;
            }

            LOG(ERROR) << "Install: exec: \"" << argv->cmd() << "\" ==> \"" << cmd << "\" check return code failed!";
        }
              
#if 0   
        ///todo:: exec a cmd not to retry when exec once failed, so the following will be ignored.
        /// error try
        if (flags & kerrtmask == kerrtry) {
            /// need retry or not?
        }

#endif 
        if ((flags & kerrxmask) == kerrignore) {
            LOG(WARNING) << "Install: exec: \"" << argv->cmd() << "\" ==> \"" << cmd << "\" failed, ignore error, continue...";
            return ERROR_Success;
        }
        
        LOG(ERROR) << "Install: exec: \"" << argv->cmd() << "\" exec failed!";
        return ERROR_ExecEntryRunFailed;
    }

    int entryInstall(SettingArgv *argv)
    {
        uint8_t flags = (uint8_t)argv->instFlags();
        if (!(flags & kinstmask)) {
            LOG(INFO) << "Install: setting: \"" << argv->flags() << "\" no need install!";
            return ERROR_Success;
        }

        shared_ptr<Setting> setting;
        int ret;
        try {
            setting.reset(new Setting(argv->flags()));
            ret = setting->doSet();
        } catch (except_base &ex) {
            ret = ex.error();          
#if 0       
            ///todo:: setting not to retry when setting failed.
            /// error try
            if (flags & kerrtmask == kerrtry) {
                /// retry once or not?.
            }
#endif 
            if ((flags & kerrxmask) == kerrignore) {
                LOG(WARNING) << "Install: setting: \"" << argv->flags() << "\" failed, ignore error, continue...";
                return ERROR_Success;
            }
        }

        if (ret != ERROR_Success)
            LOG(ERROR) << "Install: setting: \"" << argv->flags() << "\" set failed!";
        return ret;
    }

    std::string cmd_replace(std::string const& oricmd)
    {
        /// NOTE:if cmd have argv format: $GUID $xxx then find arglist, 
        ///      if find /GUID=guid then replace $GUID to guid and execute.
        ///

        std::string cmd = oricmd;
        std::string findstr;
        std::map<std::string, std::string>::iterator it;

        for (it = arg_map_.begin(); it != arg_map_.end(); ++it) {
            findstr = it->first;
            findstr[0] = '$';
            replace(cmd, findstr, it->second);
        }

        return cmd;

#if 0
        std::vector<std::string> parts;
        split(oricmd, " ", &parts, true);

        std::string cmd = "";
        for (size_t i = 0; i < parts.size(); ++i) {
            if (parts[i][0] == '$') {
                std::string tmp = parts[i];
                tmp[0] = '/';
                std::map<std::string, std::string>::iterator it;
                if ((it = arg_map_.find(tmp)) != arg_map_.end()) {
                    cmd += it->second;
                    cmd += " ";
                    continue;
                }
            }
            cmd += parts[i];
            cmd += " ";
        }

        return cmd;
#endif
    }
#pragma endregion entry install
};

} // namespace pkg

#endif // pkgtools_installer_h_