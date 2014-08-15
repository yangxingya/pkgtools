//!
/// brief: pkgdef.h define package format define.
///
/// define: package define 4 parts.
///
///     1. pkgheader
///     2. entryarray
///     3. argvlist
///     4. dataarray
///     5. data
/// details:
/// 1st:
///     header_t
///     |_ magic            // magic number for pkgtools's package.
///     |_ crc32            // all package crc32 valid.
///     |_ pkglen           // package total length.
///     |_ version          // package design version. from 1 start.
///     |_ entrymgr--|      // indicate entrymgr offset from header.
///     |_ argvmgr ----|    // indicate argvmgr offset from header.
///     |_ dinfomgr------|  // indicate dinfomgr offset from header.
///     |_ data      | | |  // indicate data offset from header.
/// 2nd:             | | |
///     entrymgr_t <-| | | 
///     |_ count       | |  // entry count.
///     |_ entrys[0]   | |  // entry array[0].
///     |_ entrys[1]   | |  //
///     |_ entrys[...] | |
///     |_ entrys[count-1]
/// 3rd:               | |
///     argvmgr_t <----| |
///     |_ count         |
///     |_ length        |
///     |_ start[length] |
/// 4th:                 |
///     dinfomgr_t <-----|
///     |_ count
///     |_ infos[...]
/// 5th:
///     data



#if !defined(pkgtools_pkg_def_h_)
#define pkgtools_pkg_def_h_

#include <cclib/types.h>

namespace pkg {

using namespace cclib;

#pragma pack(push) 
#pragma pack(1)

//! package header
/// section:
typedef struct _header_t { 
    /// magic: constant: "PKGT".
    uint32_t magic;
    /// crc32: all package data crc32 valid code.
    uint32_t crc32;
    /// pkglen: package data length, from begin to end.
    uint64_t pkglen;
    /// version: header_t version, from 1 start, if change then add 1 every time.
    uint8_t  version;
    /// compress type: 
    uint8_t  compress;
    /// entrymgr: from header_t to entrymgr distance, offset.
    uint64_t entrymgr;
    /// argvmgr: from header_t to argvmgr distance, offset.
    uint64_t argvmgr;
    /// dinfomgr: from header_t to dinfomgr distance, offset.
    uint64_t dinfomgr;
    /// data: from header_t to data distance, offset.
    uint64_t data;
} header_t;

//! entry define
/// define every can executed entry.
typedef struct _entry_t {
    /// entry type, define in entry.h
    uint16_t type;
    /// flags: define install and uninstall flags
    ///       e n u e n i
    ///   7 6 5 4 3 2 1 0
    /// detail:
    ///   5/2<e> [0/1] 1: if error ignore,  0: if error exit.
    ///   4/1<n> [0/1] 1: max effort,       0: normal
    ///   3<u> [0/1]   1: uninstall,        0: no uninstall
    ///   0<i> [0/1]   1: install,          0: no install
    /// optional
    ///   6<c> [0/1]   1: exec check return,0: exec no check return.
    uint8_t  flags;
    /// entry private argument. custom define.
    union {
        uint8_t  args4[4];
        uint16_t args2[2];
        uint32_t args;
    };
    /// string index at argvmgr.
    uint64_t strindex;
    /// data index at dinfomgr.
    uint64_t dtaindex;
} entry_t;

//! entrymgr
/// entry array manager.
typedef struct _entrymgr_t {
    /// entrys include entry count.
    uint64_t count;
    /// entry array.
    entry_t entrys[1];
} entrymgr_t;

//! argvmgr
/// argv list manager.
typedef struct _argvmgr_t {
    /// argv list count.
    uint64_t count;
    /// argv list all length.
    uint64_t length;
    /// argv list start
    /// detail: 
    ///     "argv0xxxxxx\0argv1xxxxxx\0"
    ///     ^           ^ ^          ^
    ///     b           e b          e
    /// argv[0] = "argv0xxxxxx";
    /// argv[1] = "argv1xxxxxx";
    /// count = 2;
    /// length = argv[0].length + argv[1].length + count;
    char start[1];
} argvmgr_t;

//! dinfo
/// define data cache info.
typedef struct _dinfo_t {
    /// data start from header data section, 1st start is 0.
    uint64_t start;
    /// cmplen: compress length.
    uint64_t cmplen;
    /// reallen: real length.
    uint64_t reallen;
} dinfo_t;

//! dinfomgr
/// define data info array manager
typedef struct _dinfomgr_t {
    /// count of datainfo array.
    uint64_t count;
    /// datainfo array.
    dinfo_t infos[1];
} dinfomgr_t;

#pragma pack(pop)

const size_t kheaderlen = sizeof(header_t);
const size_t kentrylen  = sizeof(entry_t);
const size_t kdinfolen  = sizeof(dinfo_t);
inline uint64_t length(header_t const* header)
{
    return sizeof(header_t);
}

inline uint64_t length(entrymgr_t const* entrymgr)
{
    uint64_t count = entrymgr->count > 0 ? (entrymgr->count - 1) : 0;
    return sizeof(entrymgr) + count * sizeof(entry_t); 
}

inline uint64_t length(argvmgr_t const* argvmgr)
{
    return argvmgr->length;
}

inline uint64_t length(dinfomgr_t const* dinfomgr)
{
    uint64_t count = dinfomgr->count > 0 ? (dinfomgr->count - 1) : 0;
    return sizeof(dinfomgr) + count * sizeof(dinfo_t);
}

const uint16_t kentryaddf  = 1;
const uint16_t kentrydelf  = 2;
const uint16_t kentrymkdir = 3;
const uint16_t kentryrmdir = 4;
const uint16_t kentryexec  = 5;
const uint16_t kentrysetting = 6;
const uint16_t kentryunknown = 0xffff;

/// uninstall shift 
const int kflagsuninshift = 3;
/// exec check return shift.
const int kflagsckshift = 7;
const uint64_t kinvalid = -1L;
/// setting int value shift.
const int kflagsintshift = 7;

const uint8_t kmagic[] = {'P', 'K', 'G', 'T'};
const uint32_t kinvalidcrc32 = 0xffffffff;
const uint8_t kcurrentver = 0x01;

const uint8_t kcompno   = 0x00;
const uint8_t kcomplzma = 0x01;


} // namespace pkg.

#endif // pkgtools_pkg_def_h_