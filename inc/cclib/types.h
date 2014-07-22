//!
/// brief: types.h define stdint.h types. (c99)
///

#if !defined(cclib_types_h_)
#define cclib_types_h_

#define CCLIB_NAMESPACE cclib
#define CCLIB_NAMESPACE_BEGIN namespace cclib {
#define CCLIB_NAMESPACE_END }

#ifdef _WIN32
# define WIN32_NAMESPACE win32
# define WIN32_NAMESPACE_BEGIN namespace win32 {
# define WIN32_NAMESPACE_END }
#endif // _WIN32

CCLIB_NAMESPACE_BEGIN

typedef unsigned char  uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int   uint32_t;
typedef unsigned __int64 uint64_t;
typedef char  int8_t;
typedef short int16_t;
typedef int   int32_t;
typedef __int64 int64_t;

const uint32_t kuint32max = 0xffffffff;
const uint32_t kuint64high32shift = 32;

const uint32_t k1kilo = 1 << 10; /// 1K.
const uint32_t k1mega = 1 << 20; /// 1M.
const uint32_t k1giga = 1 << 30; /// 1G.

#pragma warning(push)
#pragma warning(disable:4293)    /// disable compiler complain number too big.
const uint64_t k1tera = 1 << 40; /// 1T.
#pragma warning(pop)

CCLIB_NAMESPACE_END

#endif // cclib_types_h_