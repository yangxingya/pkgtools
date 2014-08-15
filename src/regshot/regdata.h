//!
/// brief: define reg data type, it's a template class.
///

#if !defined(reg_data_h_)
#define reg_data_h_

#include <string>
#include <vector>
#include <cclib/types.h>

#if 0
//!
/// @sum:  reg data type, define at:
///        http://msdn.microsoft.com/en-us/library/ms724884(v=vs.85).aspx

/// @brief: REG_BINARY
/// @desc : Binary data in any form.

/// @brief: REG_DWORD
/// @desc : A 32-bit number.

/// @brief: REG_DWORD_LITTLE_ENDIAN
/// @desc : A 32-bit number in little-endian format.
///         Windows is designed to run on little-endian computer architectures. 
///         Therefore, this value is defined as REG_DWORD in the Windows header files.

/// @brief: REG_DWORD_BIG_ENDIAN
/// @desc : A 32-bit number in big-endian format.
///         Some UNIX systems support big-endian architectures.

/// @brief: REG_EXPAND_SZ
/// @desc : A null-terminated string that contains unexpanded references to environment 
///         variables (for example, "%PATH%"). It will be a Unicode or ANSI string 
///         depending on whether you use the Unicode or ANSI functions. To expand the 
///         environment variable references, use the ExpandEnvironmentStrings function.

/// @brief: REG_LINK
/// @desc : A null-terminated Unicode string that contains the target path of a symbolic 
///         link that was created by calling the RegCreateKeyEx function with 
///         REG_OPTION_CREATE_LINK.

/// @brief: REG_MULTI_SZ
/// @desc : A sequence of null-terminated strings, terminated by an empty string (\0).
///         The following is an example:
///             String1\0String2\0String3\0LastString\0\0
///         The first \0 terminates the first string, the second to the last \0 terminates
///         the last string, and the final \0 terminates the sequence. Note that the final 
///         terminator must be factored into the length of the string.

/// @brief: REG_NONE
/// @desc : No defined value type.

/// @brief: REG_QWORD
/// @desc : A 64-bit number.

/// @brief: REG_QWORD_LITTLE_ENDIAN
/// @desc : A 64-bit number in little-endian format.
/// @desc : Windows is designed to run on little-endian computer architectures. Therefore, 
///         this value is defined as REG_QWORD in the Windows header files.

/// @brief: REG_SZ
/// @desc : A null-terminated string. This will be either a Unicode or an ANSI string, 
///         depending on whether you use the Unicode or ANSI functions.
#endif

enum RegType {
    RegNone = 0,
    RegSZ,
    RegExpandSZ,
    RegBinary,
    RegDword,
    RegDwordLiEndian,
    RegDwordBiEndian,
    RegLink,
    RegMultiSZ,
    RegResList,
    RegFullResDesc,
    RegResReqList,
    RegQword,
    RegQwordLitEndian,
};

template<RegType T>
struct RegData {
    RegData(std::string const& data) 
        : type_(T), data_(data.c_str(), data.c_str() + data.length()) 
    {
        data_.push_back('\0');
    }

    template<typename X>
    operator X()
    {
        return X(&data_[0]);
    }

private:
    RegType type_;
    std::vector<char> data_;
};

//RegData<RegSZ> regdata("test RegSZ");

#endif // reg_data_h_