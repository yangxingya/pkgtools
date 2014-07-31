//!
/// error.h: define package tools programm main function return value,
///          you can use:
///             "pkgtools.exe -e <ERRORCODE>" get detail error reason.
///

#if !defined(pkgtools_error_h_)
#define pkgtools_error_h_

#include <string>

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
    ERROR_TransferArgvError,
    ERROR_PkgHdrError,
    ERROR_CreatePkgFileFailed,
    ERROR_CustomPathGetFailed,
    ERROR_EntryTypeNotSupported,
    ERROR_DirEntryInstFailed,
    ERROR_DirEntryUninFailed,
    ERROR_ExecEntryRunFailed,
    ERROR_SettingArgsNotSupported,
    ERROR_Min = ERROR_Success,
    ERROR_Max = ERROR_SettingArgsNotSupported,
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
    "Transfer argv error",
    "Package file format error",
    "Package file create failed",
    "Custom path obtain failed",
    "Package entry type not supported",
    "Dir entry install failed",
    "Dir entry uninstall failed",
    "Exec entry run failed",
    "Setting args not supported",
};

inline const char* getError(int error)
{
    if (error < ERROR_Min || error > ERROR_Max)
        return "Unknown error code";
    return kErrorStr[error].c_str();
}

#endif // pkgtools_error_h_