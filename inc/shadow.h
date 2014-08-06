/*
 * @brief: VSS(Volume Shadow Copy Service) wrapper interface.
 *
 */

#ifndef VOLUME_SHADOW_H_
#define VOLUME_SHADOW_H_

#define VSS_SHARED 1

// VSS_EXPORTS

#if !defined(VSS_SHARED)
# define VSS_STATIC 1
#endif // no defined VSS_SHARED

// define VSS_EXPORTS in COMPLIER options, not in include.
// #define VSS_EXPORTS 1

// define to static library.
#ifdef VSS_STATIC
# define VSS_API extern "C"
#else
// define to shared library.
# ifdef VSS_SHARED
#  ifdef VSS_EXPORTS
#   define VSS_API extern "C" __declspec(dllexport)
#  else
#   define VSS_API extern "C" __declspec(dllimport)
#  endif // VSS_EXPORTS
# endif  // VSS_SHARED
#endif 

// forward declare, for VssHandle use.
struct ShadowCopy;
typedef ShadowCopy *VssHandle;

//
// @brief : create volume shadow copy
// @param : volume-> disk volume, form: "C:\\"
//          NOTICE: you MUST end volume string with '\\'.
// @return: if function call succeed, then rethrn no NULL
//          handle, else return NULL.
//
VSS_API
VssHandle CreateShadowCopy(const wchar_t *volume);

//
// @brief : release volume shadow copy
// @param : handle-> it MUST be the return of call
//          CreateShadowCopy. 
// @return: void.
//
VSS_API
void ReleaseShadowCopy(const VssHandle handle);

//
// @brief : get volume shadow copy path
// @param : handle-> it MUST be the return of call
//          CreateShadowCopy. 
// @return: the path of volume shadow copy path.
//          you can use it to access file.
//          form: "\\?\GLOBALROOT\Device\HarddistVolumeShadowCopy<xx>"
//          <xx> is a number.
//
VSS_API
const wchar_t* GetShadowCopyPath(const VssHandle handle);

//
// @brief : get last error if call CreateShadowCopy failed!
// @param : no.
// @return: the string of last error. 
//          NOTICE: it isn't THREAD SAFETY!
//
VSS_API
const wchar_t* LastError(void);

const char FunName_CreateShadowCopy [] = "CreateShadowCopy";
const char FunName_ReleaseShadowCopy[] = "ReleaseShadowCopy";
const char FunName_GetShadowCopyPath[] = "GetShadowCopyPath";
const char FunName_LastError[]         = "LastError";

typedef VssHandle      (*Fun_CreateShadowCopy )(const wchar_t *);
typedef void           (*Fun_ReleaseShadowCopy)(const VssHandle);
typedef const wchar_t* (*Fun_GetShadowCopyPath)(const VssHandle);
typedef const wchar_t* (*Fun_LastError)        (void);

#endif // VOLUME_SHADOW_H_