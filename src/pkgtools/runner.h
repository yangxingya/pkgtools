//!
/// brief: runner.h define all entry can be run.
///

#if !defined(pkgtools_runner_h_)
#define pkgtools_runner_h_

#include <string>
#include <cclib/types.h>
#include <cclib/strutil.h>
#include <cclib/win32/executer.h>
#include "except.h"
#include "unpacker.h"
#include "argv.h"
#include "argvdef.h"

namespace pkg {

using namespace err;
using namespace cclib;
using namespace argv;

enum operate_t {
    OP_Package,
    OP_Install,
    OP_Uninstall,
    OP_Extract,
};

const char kplaceholder = '%';

struct runner 
{
    runner(unpacker &unpacker, operate_t op, std::map<std::string, std::string> &arg_map)
        : unpacker_(unpacker), op_(op), arg_map_(arg_map), extr_(op == OP_Extract) 
    {
        if (op_ == OP_Package) {
            std::string err = "runner not supported package operate.";
            LOG(ERROR) << err;
            throw run_error(ERROR_RunnerNotSupported, err);
        }
    }

    void operator()(AutoArgv argv)
    {
        inst_ = !!(argv->instFlags() & kinstmask);
        unin_ = !!(argv->uninFlags() & kinstmask);

        bool errtry = inst_ ? (!!(argv->instFlags() & kerrtmask)) : (!!(argv->uninFlags() & kerrtmask));
        bool igrerr = inst_ ? (!!(argv->instFlags() & kerrxmask)) : (!!(argv->uninFlags() & kerrxmask));
        
        switch (argv->type()) {
            case kAddf:    run<AddfArgv> (argv, errtry, igrerr); break;
            case kDelf:    run<DelfArgv> (argv, errtry, igrerr); break;
            case kMkdir:   run<MkdirArgv>(argv, errtry, igrerr); break;
            case kRmdir:   run<RmdirArgv>(argv, errtry, igrerr); break;
            case kExec:    run<ExecArgv> (argv, errtry, igrerr); break;
            case kSetting: run<SettingArgv> (argv, errtry, igrerr); break;
        }
    }

private:
    unpacker &unpacker_;
    operate_t op_;

    bool inst_;
    bool unin_;
    bool extr_;

    std::map<std::string, std::string> &arg_map_;

    template<typename T>
    void run(AutoArgv argv, bool errtry, bool igrerr)
    {
        T *fargv = (T*)argv.get();
        switch (op_) {
        case OP_Install:
            if (inst_)
                run(fargv, errtry, igrerr);
            break;
        case OP_Uninstall:
            if (unin_)
                run(fargv, errtry, igrerr);
            break;
        case OP_Extract:
            if (extr_) {
                /// extract only addf and mkdir to special out dir.
                if (typeid(T) == typeid(AddfArgv) || typeid(T) == typeid(MkdirArgv))
                    run(fargv, errtry, igrerr);
            }
            break;
        default:
            std::string err = "runner not supported operate.";
            LOG(ERROR) << err;
            throw run_error(ERROR_RunnerNotSupported, err);
        }
    }


#pragma region detail run
    void run(AddfArgv *argv, bool errtry, bool ignorerr)
    {
        uint64_t len = unpacker_.tofile(argv->dst(), argv->offset());
        
        if (len != 0) {
            LOG(INFO) << "runner add file: \"" << argv->dst() << "\" RAW copy successful!";
            return;
        }

        /// error try
        if (errtry) {
            /// need remove file dir relatived right.<TrustedInstaller right>.
            
        }

        /// error ignore
        if (ignorerr) {
            LOG(WARNING) << "runner add file: \"" << argv->dst() << "\" copy failed, ignore error, continue...";
            return;
        }

        std::string err = "runner add file: \"";
        err += argv->dst();
        err += "\" copy failed!";
        LOG(ERROR) << err;
        throw run_error(ERROR_AddFileFailed, err);
    }

    void run(DelfArgv *argv, bool errtry, bool ignorerr)
    {
        /// support install.
        if (win32::rmfile(argv->dst())) {
            LOG(INFO) << "runner del file: \"" << argv->dst() << "\" successful!";
            return;
        } 

        /// error try
        if (errtry) {
            /// need remove file dir relatived right.<TrustedInstaller right>.

        }

        /// error ignore
        if (ignorerr) {
            LOG(WARNING) << "runner del file: \"" << argv->dst() << "\" failed, ignore error, continue...";
            return;
        }

        std::string err = "runner del file: \"";
        err += argv->dst();
        err += "\" failed!";
        LOG(ERROR) << err;
        throw run_error(ERROR_DeleteFileFailed, err);
    }

    void run(MkdirArgv *argv, bool errtry, bool ignorerr)
    {
        if (win32::mkdirtree(argv->dst())) {
            LOG(INFO) << "runner mkdir: \"" << argv->dst() << "\" successful!";
            return;
        }
        
        /// error try
        if (errtry) {
            /// need remove parent dir relatived right.<TrustedInstaller right>.
            std::string dir = argv->dst();
            if (win32::is_special(dir)) {
                std::string pdir = win32::pdir(dir);
                if (win32::rm_trustedinstaller(pdir)) {
                    LOG(INFO) << "runner mkdir: remove parent dir trustedinstaller : \"" << pdir << "\" successful!";    
                    if (win32::mkdirtree(argv->dst())) {
                        LOG(INFO) << "runner mkdir: \"" << argv->dst() << "\" successful!";
                        return;
                    }
                    LOG(ERROR) << "runner mkdir: remove parent dir trustedinstaller, but mkdir failed : \"" << pdir << "\"!";
                }
            }
        }

        /// error ignore
        if (ignorerr) {
            LOG(WARNING) << "runner mkdir: \"" << argv->dst() << "\" failed, ignore error, continue...";
            return;
        }

        std::string err = "runner mkdir: \"";
        err += argv->dst();
        err += "\" failed!";
        LOG(ERROR) << err;
        throw run_error(ERROR_MakeDirFailed, err);
    }

    void run(RmdirArgv *argv, bool errtry, bool ignorerr)
    {
        if (win32::rmdir(argv->dst())) {
            LOG(INFO) << "runner rmdir: \"" << argv->dst() << "\" successful!";
            return;
        }
        
        /// error try
        if (errtry) {
            /// need remove parent dir relatived right.<TrustedInstaller right>.

        }

        /// error ignore
        if (ignorerr) {
            LOG(WARNING) << "runner rmdir: \"" << argv->dst() << "\" failed, ignore error, continue...";
            return;
        }

        std::string err = "runner rmdir: \"";
        err += argv->dst();
        err += "\" failed!";
        LOG(ERROR) << err;
        throw run_error(ERROR_RemoveDirFailed, err);
    }

    void run(ExecArgv *argv, bool errtry, bool ignorerr)
    {
        std::string cmd = cmd_replace(argv->cmd());
        
        LOG(INFO) << "runner exec: raw cmdline: \"" << argv->cmd() << "\"";

        uint32_t retcode;
        if (win32::execute(&retcode, "", cmd)) {
            LOG(INFO) << "runner exec: \"" << cmd << "\" execute successful!";

            if (!argv->ckFlags()) {
                LOG(INFO) << "runner exec: \"" << cmd << "\" no need check return code";
                return;
            }
            
            /// need check return.
            if (argv->ckReturn() == retcode) {
                LOG(INFO) << "runner exec: \"" << cmd << "\"  check return code successful!";
                return;
            }

            std::stringstream ss;
            ss  << "check code: " << argv->ckReturn()
                << ", exec return code: " << retcode;
            std::string codestr;
            ss  >> codestr;

            std::string err = "runner exec: \"";
            err += cmd;
            err += "\" failed! ";
            err += codestr;

            LOG(ERROR) << err;
            throw run_error(ERROR_ExecEntryRunFailed, err);
        }

#if 0   
        ///todo:: exec a cmd not to retry when exec once failed, so the following will be ignored.
        /// error try
        if (errtry) {
            /// need retry or not?
        }

#endif 
        if (ignorerr) {
            LOG(WARNING) << "runner exec: \"" << cmd << "\" failed, ignore error, continue...";
            return;
        }

        std::string err = "runner exec: \"";
        err += cmd;
        err += "\" failed! ";

        LOG(ERROR) << err;
        throw run_error(ERROR_ExecEntryRunFailed, err);
    }

    void run(SettingArgv *argv, bool errtry, bool ignorerr)
    {
        shared_ptr<Setting> setting;
        int ret;
        try {
            setting.reset(new Setting(argv->flags()));
            ret = setting->doSet();
            if (ret == ERROR_Success) {
                LOG(INFO) << "runner setting: \"" << argv->flags() << "\" successful!";
                return;
            }
        } catch (except_base &ex) {
            ret = ex.error();          
        }

#if 0       
        ///todo:: setting not to retry when setting failed.
        /// error try
        if (errtry) {
            /// retry once or not?.
        }
#endif 
        if (ignorerr) {
            LOG(WARNING) << "runner setting: \"" << argv->flags() << "\" failed, ignore error, continue...";
            return;
        }

        std::string err = "runner exec: \"";
        err += argv->flags();
        err += "\" failed! ";

        LOG(ERROR) << err;
        throw run_error(ret, err);
    }

    std::string cmd_replace(std::string const& oricmd)
    {
        /// NOTE:if cmd have argv format: %GUID %xxx then find arglist, 
        ///      if find /GUID=guid then replace %GUID to guid and execute.
        ///

        std::string cmd = oricmd;
        std::string findstr;
        std::map<std::string, std::string>::iterator it;

        for (it = arg_map_.begin(); it != arg_map_.end(); ++it) {
            findstr = it->first;
            findstr[0] = kplaceholder;
            replace(cmd, findstr, it->second);
        }

        return cmd;
    }

#pragma endregion detail run

};

} // namespace pkg

#endif // pkgtools_runner_h_