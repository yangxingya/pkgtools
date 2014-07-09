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
    ERROR_CreatePkgFileFailed,
    ERROR_Min = ERROR_Success,
    ERROR_Max = ERROR_CreatePkgFileFailed,
};

const std::string kErrorStr[] = {
    "Successful!",
    "Usage invalid",
    "No enough space while Installing",
    "No content in Script",
    "Script line no entry split char(:)",
    "Script line entry type not supported",
    "Package file create failed",
};

inline const char* getError(int error)
{
    if (error < ERROR_Min || error > ERROR_Max)
        return "Unknown error code";
    return kErrorStr[error].c_str();
}

#endif // pkgtools_error_h_