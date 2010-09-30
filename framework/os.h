#ifndef OS_H
#define OS_H
//(c) Adrian Boeing 2004, see liscence.txt (BSD liscence)
/**
	Abstract:
		Redefined functions for a specific OS
	Author:
		Adrian Boeing
	Revision History:
		Version 1.2.1:11/12/07 OSX support CDECL and linux DLL
		Version 1.2  :06/12/07 Cross platfrom DLL
		Version 1.1.2:20/03/05 VC8 *sprintf
		Version 1.1.1:14/01/04 Fixed linux support issue
		Version 1.1 : 18/11/03 Crical Message Update (NotFixed)
		Version 1.0 : 15/11/03 Initial
	TODO:
		-Fix win32 critical message service notification
*/
#include "common.h"

#include <stdio.h>
#include <stdarg.h>

#ifndef __TIMESTAMP__
#define __TIMESTAMP__ (__DATE__ " " __TIME__)
#endif

// opaque class used for a dynamic lib handle.
class OS_DynlibHandleClass;
typedef OS_DynlibHandleClass* OS_DynlibHandle;

void OS_Sleep(unsigned milisec);
void OS_CriticalMessage(const char* msg);
OS_DynlibHandle DYNLIB_LOAD(const char* file);
void* DYNLIB_GETSYM(OS_DynlibHandle handle, const char* symbolName);
bool DYNLIB_UNLOAD(OS_DynlibHandle handle);

#if defined (OS_WINDOWS) || defined(_WIN32)
#undef BOOL
#undef BYTE
#undef WORD
#undef DWORD
#undef FLOAT

// MSVC has deprecated these and tells you to use the _ versions
#define strdup _strdup
#define stricmp _stricmp

#ifndef NOMINMAX
#define NOMINMAX
#endif

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifdef MICROSOFT_VC_8
#include <stdio.h>
#define OS_snprintf sprintf_s
#define OS_vsnprintf vsprintf_s
#else
#define OS_vsnprintf _vsnprintf
#define OS_snprintf _snprintf
#endif

#elif defined (OS_LINUX) || defined(OS_SOLARIS) || defined(OS_OSX) || defined(__CYGWIN__)
#define OS_vsnprintf vsnprintf
#define OS_snprintf snprintf
#endif

#undef CDECL
#if defined (OS_WINDOWS) || defined(_WIN32)
#define DLL_FUNC __declspec(dllexport)
#define CDECL _cdecl
#else
#define CDECL
#define DLL_FUNC
#endif

#endif
