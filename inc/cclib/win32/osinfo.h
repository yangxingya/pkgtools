//!
/// brief: osinfo.h define windows os information
///

#if !defined(cclib_win32_os_info_h_)
#define cclib_win32_os_info_h_

#include <string>
#include <sstream>
#include <windows.h>
#include "../types.h"

CCLIB_NAMESPACE_BEGIN
WIN32_NAMESPACE_BEGIN

/// intel x86(32bit)
const int kCpuArchX86 = 1; 
/// intel x64 or amd x64
const int kCpuArchX64 = 2;
/// intel ia64
const int kCpuArchIA64 = 3;
/// arm 
const int kCpuArchARM = 4;
/// unknown
const int kCpuArchUnknown = 0xffff;

/// the following is for test
inline std::string cpuArch(const SYSTEM_INFO *lpsi)
{
    std::string arch;
    switch (lpsi->wProcessorArchitecture){
    case PROCESSOR_ARCHITECTURE_INTEL:
        arch = "Intel";
        break;
    case PROCESSOR_ARCHITECTURE_MIPS:
        arch = "MIPS";
        break;
    case PROCESSOR_ARCHITECTURE_ALPHA:
        arch = "Alpha";
        break;
    case PROCESSOR_ARCHITECTURE_PPC:
        arch = "PPC";
        break;
    case PROCESSOR_ARCHITECTURE_SHX:
        arch = "SHx";
        break;
    case PROCESSOR_ARCHITECTURE_ARM:
        arch = "ARM";
        break;
    case PROCESSOR_ARCHITECTURE_IA64:
        arch = "IA64";
        break;
    case PROCESSOR_ARCHITECTURE_ALPHA64:
        arch = "Alpha64";
        break;
    case PROCESSOR_ARCHITECTURE_MSIL:
        arch = "MSIL";
        break;
    case PROCESSOR_ARCHITECTURE_AMD64:
        arch = "AMD64";
        break;
    case PROCESSOR_ARCHITECTURE_IA32_ON_WIN64:
        arch = "IA32/WIN64";
        break;
    default:
        std::stringstream ss;
        ss << "0x" << std::hex << lpsi->wProcessorArchitecture;
        ss >> arch;
        break;
    }
    return arch;
}

inline std::string cpuType(const SYSTEM_INFO *lpsi)
{
    std::string cpu;
    switch (lpsi->dwProcessorType) {
    case PROCESSOR_INTEL_386:
        cpu = "Intel 386";
        break;
    case PROCESSOR_INTEL_486:
        cpu = "Intel 486";
        break;
    case PROCESSOR_INTEL_PENTIUM:
        cpu = "Intel Pentium";
        break;
    case PROCESSOR_INTEL_IA64:
        cpu = "Intel IA64";
        break;
    case PROCESSOR_AMD_X8664:
        cpu = "AMD X86-64";
        break;
    case PROCESSOR_MIPS_R4000:
        cpu = "MIPS R4000";
        break;
    case PROCESSOR_ALPHA_21064:
        cpu = "Alpha 2164";
        break;
    case PROCESSOR_PPC_601:
        cpu = "PPC 601";
        break;
    case PROCESSOR_PPC_603:
        cpu = "PPC 603";
        break;
    case PROCESSOR_PPC_604:
        cpu = "PPC 604";
        break;
    case PROCESSOR_PPC_620:
        cpu = "PPC 620";
        break;
    case PROCESSOR_HITACHI_SH3:
        cpu = "Hitachi SH3";
        break;
    case PROCESSOR_HITACHI_SH3E:
        cpu = "Hitachi SH3E";
        break;
    case PROCESSOR_HITACHI_SH4:
        cpu = "Hitachi SH4";
        break;
    case PROCESSOR_MOTOROLA_821:
        cpu = "Motorola 821";
        break;
    case PROCESSOR_SHx_SH3:
        cpu = "SHx-SH3";
        break;
    case PROCESSOR_SHx_SH4:
        cpu = "SHx-SH4";
        break;
    case PROCESSOR_STRONGARM:
        cpu = "StrongARM";
        break;
    case PROCESSOR_ARM720:
        cpu = "ARM-720";
        break;
    case PROCESSOR_ARM820:
        cpu = "ARM-820";
        break;
    case PROCESSOR_ARM920:
        cpu = "ARM-920";
        break;
    case PROCESSOR_ARM_7TDMI:
        cpu = "ARM-7TDMI";
        break;
    case PROCESSOR_OPTIL:
        cpu = "OPTIL";
        break;
    default:
        std::stringstream ss;
        ss << "0x" << std::hex << lpsi->dwProcessorType;
        ss >> cpu;
        break;
    }
    return cpu;
}

/// is 64bit os.
inline bool is64bitOS()
{
    SYSTEM_INFO sysinfo;

    /// the following func can be used XP or 2003 later.
    /// detail: 
    ///     http://msdn.microsoft.com/en-us/library/windows/desktop/ms724340(v=vs.85).aspx
    GetNativeSystemInfo(&sysinfo);
    return sysinfo.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64;
}

/// self is 64bit?
inline bool is64bitSelf()
{
    return sizeof(void*) == 8;
}

uint64_t osver();

//version overview: http://msdn.microsoft.com/en-us/library/ms724834(VS.85).aspx
const uint64_t koswin81   = makeullong(6, 3);
const uint64_t koswin8    = makeullong(6, 2);
const uint64_t koswin7    = makeullong(6, 1);
const uint64_t kosvista   = makeullong(6, 0);
const uint64_t koswin2003 = makeullong(5, 2);
const uint64_t koswinxp   = makeullong(5, 1);

inline bool win81   () { return osver() >= koswin81;   }
inline bool win8    () { return osver() >= koswin8;    }
inline bool win7    () { return osver() >= koswin7;    }
inline bool winVista() { return osver() >= kosvista;   }
inline bool win2003 () { return osver() >= koswin2003; }
inline bool winXp   () { return osver() >= koswinxp;   }

inline uint64_t osver()
{
    OSVERSIONINFO osvi = {};
    osvi.dwOSVersionInfoSize = sizeof(osvi);
    if (!::GetVersionEx(&osvi))
        return 0L;
    return makeullong(osvi.dwMajorVersion, osvi.dwMinorVersion);
}

WIN32_NAMESPACE_END
CCLIB_NAMESPACE_END

#endif // cclib_win32_os_info_h_