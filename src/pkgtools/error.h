//!
/// error.h: define package tools programm main function return value,
///          you can use:
///             "pkgtools.exe -e <ERRORCODE>" get detail error reason.
///

#if !defined(pkgtools_error_h_)
#define pkgtools_error_h_

#include <string>

enum Error {
    ERROR_Success,
    ERROR_UsageInvalid,
    ERROR_NoEnoughSpaceWhileInstalling, 

};

const std::string kErrorStr[] = {
    "Successful!",
    "Usage invalid",
    "No enough space while Installing",
};

inline const char* getError(Error error)
{
    return kErrorStr[error].c_str();
}


#endif // pkgtools_error_h_