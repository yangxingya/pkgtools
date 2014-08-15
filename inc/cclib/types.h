//!
/// brief: types.h define stdint.h types. (c99)
///

#if !defined(cclib_types_h_)
#define cclib_types_h_

#define CCLIB_NAMESPACE cclib
#define CCLIB_NAMESPACE_BEGIN namespace cclib {
#define CCLIB_NAMESPACE_END }

#define ALGO_NAMESPACE algo
#define ALGO_NAMESPACE_BEGIN namespace algo {
#define ALGO_NAMESPACE_END }

#ifdef _WIN32
# define WIN32_NAMESPACE win32
# define WIN32_NAMESPACE_BEGIN namespace win32 {
# define WIN32_NAMESPACE_END }
#endif // _WIN32

#define DUMMY

CCLIB_NAMESPACE_BEGIN

typedef unsigned char  uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int   uint32_t;
typedef unsigned __int64 uint64_t;
typedef char  int8_t;
typedef short int16_t;
typedef int   int32_t;
typedef __int64 int64_t;

typedef long long_t;
typedef unsigned long ulong_t;

typedef unsigned char byte_t;

const uint32_t kuint32max = 0xffffffff;
const uint32_t kuint64high32shift = 32;

const uint32_t k1kilo = 1 << 10; /// 1K.
const uint32_t k1mega = 1 << 20; /// 1M.
const uint32_t k1giga = 1 << 30; /// 1G.

#pragma warning(push)
#pragma warning(disable:4293)    /// disable compiler complain number too big.
const uint64_t k1tera = 1 << 40; /// 1T.
#pragma warning(pop)

typedef union _llong_t {
    struct {
        ulong_t low;
        long_t high;
    } DUMMY;
    struct {
        ulong_t low;
        long_t high;
    } u;
    int64_t quad;
} llong_t;

typedef union _ullong_t {
    struct {
        ulong_t low;
        ulong_t high;
    } DUMMY;
    struct {
        ulong_t low;
        ulong_t high;
    } u;
    uint64_t quad;
} ullong_t;

inline uint64_t makeullong(ulong_t high, ulong_t low)
{
    ullong_t tmp = {};
    tmp.high = high;
    tmp.low  = low;

    // c++11 supported static_assert. vc2010 and later supported.
    //static_assert(sizeof(tmp) == sizeof(uint64_t), "");
    return tmp.quad;
}

inline int64_t makellong(long_t high, ulong_t low)
{
    llong_t tmp = {};
    tmp.high = high;
    tmp.low  = low;

    // c++11 supported static_assert. vc2010 and later supported.
    //static_assert(sizeof(tmp) == sizeof(int64_t), "");
    return tmp.quad;
}

CCLIB_NAMESPACE_END

#endif // cclib_types_h_
