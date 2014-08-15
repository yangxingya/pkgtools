//! except.h is define all throwed exception
///
///

#if !defined(pkgtools_except_h_)
#define pkgtools_except_h_

#include <string>

namespace err {

enum Error {
    ERROR_Success = 0,
    ERROR_UsageInvalid,
    ERROR_NoEnoughSpaceWhileInstalling, 
    ERROR_NoContentInScript,
    ERROR_ScriptLineNoEntrySplitChar,
    ERROR_EntryTypeCantSupported,
    ERROR_ScriptFormatError,
    ERROR_ScriptNoOutArgs,
    ERROR_ScriptOutPathFormatError,
    ERROR_ScriptSettingFormatError,
    ERROR_ScriptFilePathFormatError,
    ERROR_ScriptDirPathFormatError,
    ERROR_ScriptExecCmdFormatError,
    ERROR_SpecFolderGetError,
    ERROR_OpenFileFailed,
    ERROR_CheckFileError,
    ERROR_TransferArgvError,
    ERROR_PkgHdrError,
    ERROR_CreatePkgFileFailed,
    ERROR_CustomPathGetFailed,
    ERROR_EntryTypeNotSupported,
    ERROR_AddFileFailed,
    ERROR_DeleteFileFailed,
    ERROR_MakeDirFailed,
    ERROR_RemoveDirFailed,
    ERROR_ExecEntryRunFailed,
    ERROR_SettingArgsNotSupported,
    ERROR_RunnerNotSupported,
    ERROR_CalcCrc32Failed,
    ERROR_Crc32CheckFailed,
    ERROR_Min = ERROR_Success,
    ERROR_Max = ERROR_CalcCrc32Failed,
};

const std::string kErrorStr[] = {
    "Successful!",
    "Usage invalid",
    "No enough space while Installing",
    "No content in Script",
    "Script line no entry split char(:)",
    "Script line entry type not supported",
    "Script format error",
    "Script no out args",
    "Script out path argv format error",
    "Script setting format error",
    "Script file path argv format error",
    "Script dir path argv format error",
    "Script exec cmd argv format error",
    "Special folder get error",
    "Open file failed",
    "Check file error",
    "Transfer argv error",
    "Package file format error",
    "Package file create failed",
    "Custom path obtain failed",
    "Package entry type not supported",
    "Add file failed",
    "Delete file failed",
    "Make dir failed",
    "Remove dir failed",
    "Exec entry run failed",
    "Setting args not supported",
    "Runner not support operate type",
    "Calc crc32 failed",
    "Crc32 check failed",
};

inline const char* getError(int error)
{
    if (error < ERROR_Min || error > ERROR_Max)
        return "Unknown error code";
    return kErrorStr[error].c_str();
}

struct except_base 
{
    except_base(int errcode, std::string const& what)
        : errcode_(errcode), what_(what) {}
    virtual ~except_base(){}
    const char* what() const { return what_.c_str(); }
    int error() const { return errcode_; }
protected:
    std::string what_;
    int errcode_;
};

struct no_impl : public except_base
{
    no_impl(int errcode = -1, std::string const& what = "not implement")
        : except_base(errcode, what) {}
};

struct script_error : public except_base
{
    script_error(int errcode = -1, std::string const& what = "script format error")
        : except_base(errcode, what) {}
};

struct pkg_error : public except_base
{
    pkg_error(int errcode = -1, std::string const& what = "make package error")
        : except_base(errcode, what) {}
};

struct inst_error : public except_base
{
    inst_error(int errcode = -1, std::string const& what = "install error")
        : except_base(errcode, what) {}
};

struct uninst_error : public except_base
{
    uninst_error(int errcode = -1, std::string const& what = "uninstall error")
        : except_base(errcode, what) {}
};

struct extract_error : public except_base
{
    extract_error(int errcode = -1, std::string const& what = "extract package error")
        : except_base(errcode, what) {}
};

struct spget_error : public except_base
{
    spget_error(int errcode = -1, std::string const& what = "special folder get error")
        : except_base(errcode, what) {}
};

struct setting_error : public except_base
{
    setting_error(int errcode = -1, std::string const& what = "setting error")
        : except_base(errcode, what) {}
};

struct run_error : public except_base
{
    run_error(int errcode = -1, std::string const& what = "runner error")
        : except_base(errcode, what) {}
};

} // namespace err;

#endif // pkgtools_except_h_