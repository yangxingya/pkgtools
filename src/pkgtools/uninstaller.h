//!
/// brief: uninstaller.h define uninstall a installed software.
///

#if !defined(pkgtools_uninstaller_h_)
#define pkgtools_uninstaller_h_

#include <string>
#include <cclib/types.h>
#include "unpacker.h"
#include "pkgfile.h"
#include "argv.h"
#include "argvdef.h"

namespace pkg {

using namespace cclib;
using namespace argv;

struct Uninstaller
{
    Uninstaller(std::string const& pkgfile)
        : unpacker_(pkgfile)
    { }

    int uninstall()
    {
        std::vector<entry_t> const& entrys = unpacker_.entrys();
        std::vector<std::string> const& args = unpacker_.arglist();

        AutoArgvList arglist;
        entry::restorer restor(args, arglist);
        std::for_each(entrys.begin(), entrys.end(), restor);
    
        int ret;
        for (size_t i = 0; i < arglist.size(); ++i) {
            if ((ret = uninstall(arglist[arglist.size() - 1 - i])) != ERROR_Success)
                return ret;
            
        }
        return ERROR_Success;
    }

private:
    unpacker unpacker_;

#pragma region entry uninstall
    int uninstall(AutoArgv argv)
    {
        switch (argv->type()) {
        case entry::kFile: return entryUninstall((FileArgv*)argv.get());
        case entry::kDir:  return entryUninstall((DirArgv*) argv.get());
        case entry::kExec: return entryUninstall((ExecArgv*)argv.get());
        default: return ERROR_EntryTypeNotSupported;
        }
    }

    int entryUninstall(FileArgv *argv)
    {
        uint8_t flags = (uint8_t)argv->uninFlags();

        if (!(flags & kuninmask)) {
            LOG(INFO) << "Uninstall: file: \"" << argv->dst() << "\" no need uninstall";
            return ERROR_Success;
        }

        int ret;
        /// support uninstall.
        try {
            if (argv->deleted()) {
                ///todo:: file uninstall will be removed.
                win32::rmfile(argv->dst());
                LOG(INFO) << "Uninstall: file: \"" << argv->dst() << "\" RAW remove successful!";
            } else {
                unpacker_.tofile(argv->dst(), argv->offset());
                LOG(INFO) << "Uninstall: file: \"" << argv->dst() << "\" extract successful!";
            }
            
            return ERROR_Success;
        } catch (except_base &ex) {
            ret = ex.error();
            LOG(WARNING) << "Uninstall: file: \"" << argv->dst() << "\" RAW remove failed!!!";
        }

        /// error try
        if ((flags & kerrtmask) == kerrtry) {
            /// need remove file dir relatived right.<TrustedInstaller right>.

        }

        /// error ignore
        if ((flags & kerrxmask) == kerrignore) {
            LOG(WARNING) << "Uninstall: file: \"" << argv->dst() << "\" remove failed, ignore error, continue...";
            return ERROR_Success;
        }

        LOG(ERROR) << "Uninstall: file: \"" << argv->dst() << "\" remove failed!";
        return ret;
    }

    int entryUninstall(DirArgv *argv)
    {
        uint8_t flags = (uint8_t)argv->uninFlags();
        if (!(flags & kuninmask)) {
            LOG(INFO) << "Uninstall: dir: \"" << argv->dst() << "\" no need uninstall";
            return ERROR_Success;
        }

        if (win32::rmdir(argv->dst())) {
            LOG(INFO) << "Uninstall: dir: \"" << argv->dst() << "\" remove successful!";
            return ERROR_Success;
        }
        
        /// error try
        if ((flags & kerrtmask) == kerrtry) {
            /// need remove parent dir relatived right.<TrustedInstaller right>.

        }

        /// error ignore
        if ((flags & kerrxmask) == kerrignore) {
            LOG(WARNING) << "Uninstall: dir: \"" << argv->dst() << "\" remove failed, ignore error, continue...";
            return ERROR_Success;
        }

        LOG(ERROR) << "Uninstall: dir: \"" << argv->dst() << "\" remove failed!";
        return ERROR_DirEntryUninFailed;
    }

    int entryUninstall(ExecArgv *argv)
    {
        uint8_t flags = (uint8_t)argv->uninFlags();
        if (!(flags & kuninmask)) {
            LOG(INFO) << "Uninstall: exec: \"" << argv->cmd() << "\" no need uninstall";
            return ERROR_Success;
        }

        uint32_t retcode;
        if (win32::execute(&retcode, "", argv->cmd())) {
            LOG(INFO) << "Uninstall: exec: \"" << argv->cmd() << "\" execute successful!";

            if (!argv->ckFlags()) {
                LOG(INFO) << "Uninstall: exec: \"" << argv->cmd() << "\" no need check return code";
                return ERROR_Success;
            }
            
            /// need check return.
            if (argv->ckReturn() == retcode) {
                LOG(INFO) << "Uninstall: exec: \"" << argv->cmd() << "\" check return code successful!";
                return ERROR_Success;
            }
            
            LOG(ERROR) << "Uninstall: exec: \"" << argv->cmd() << "\" check return code failed!";
        }
        
        ///todo:: exec a cmd not to retry when exec once failed, so the following will be ignored.
#if 0       
        /// error try
        if (flags & kerrtmask == kerrtry) {
            /// need remove parent dir relatived right.<TrustedInstaller right>.
        }

        /// error ignore
        if (flags & kerrxmask == kerrignore) {
            LOG(WARNING) << "Uninstall: dir: \"" << argv->dst() << "\" create failed, ignore error, continue...";
            return ERROR_Success;
        }

        LOG(ERROR) << "Uninstall: dir: \"" << dargv->dst() << "\" create failed!";
        return ret;
#endif 
        
        LOG(ERROR) << "Uninstall: exec: \"" << argv->cmd() << "\" exec failed!";
        return ERROR_ExecEntryRunFailed;
    }
#pragma endregion entry install
};

} // namespace pkg

#endif // pkgtools_uninstaller_h_