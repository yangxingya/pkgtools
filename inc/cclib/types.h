//!
/// brief: types.h define stdint.h types. (c99)
///

#if !defined(cclib_types_h_)
#define cclib_types_h_

#define CCLIB_NAMESPACE cclib
#define CCLIB_NAMESPACE_BEGIN namespace cclib {
#define CCLIB_NAMESPACE_END }

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

CCLIB_NAMESPACE_END


#endif // cclib_types_h_