//!
/// brief: installer.h define install a package to pc.
///

#if !defined(pkgtools_installer_h_)
#define pkgtools_installer_h_

#include <string>
#include <cclib/types.h>
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
    Installer(std::string const& pkgfile)
        : unpacker_(pkgfile) {}

    int install()
    {
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

#pragma region entry install
    int install(AutoArgv argv)
    {
        switch (argv->type()) {
        case entry::kFile: return entryInstall((FileArgv*)argv.get());
        case entry::kDir:  return entryInstall((DirArgv*) argv.get());
        case entry::kExec: return entryInstall((ExecArgv*)argv.get());
        default: return ERROR_EntryTypeNotSupported;
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

        uint32_t retcode;
        if (win32::execute(&retcode, "", argv->cmd())) {
            LOG(INFO) << "Install: exec: \"" << argv->cmd() << "\" execute successful!";

            if (!argv->ckFlags()) {
                LOG(INFO) << "Install: exec: \"" << argv->cmd() << "\" no need check return code";
                return ERROR_Success;
            }
            
            /// need check return.
            if (argv->ckReturn() == retcode) {
                LOG(INFO) << "Install: exec: \"" << argv->cmd() << "\" check return code successful!";
                return ERROR_Success;
            }
            
            LOG(ERROR) << "Install: exec: \"" << argv->cmd() << "\" check return code failed!";
        }
        
        ///todo:: exec a cmd not to retry when exec once failed, so the following will be ignored.
#if 0       
        /// error try
        if (flags & kerrtmask == kerrtry) {
            /// need remove parent dir relatived right.<TrustedInstaller right>.
        }

        /// error ignore
        if (flags & kerrxmask == kerrignore) {
            LOG(WARNING) << "Install: dir: \"" << argv->dst() << "\" create failed, ignore error, continue...";
            return ERROR_Success;
        }

        LOG(ERROR) << "Install: dir: \"" << dargv->dst() << "\" create failed!";
        return ret;
#endif 
        
        LOG(ERROR) << "Install: exec: \"" << argv->cmd() << "\" exec failed!";
        return ERROR_ExecEntryRunFailed;
    }
#pragma endregion entry install
};

} // namespace pkg

#endif // pkgtools_installer_h_