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

#if defined(OS_WINDOWS) || defined(WIN32)

#undef BOOL
#undef BYTE
#undef WORD
#undef DWORD
#undef FLOAT
#include <windows.h>

#ifdef MICROSOFT_VC_8
#define OS_snprintf sprintf_s
#define OS_vsnprintf vsprintf_s
#else
#define OS_vsnprintf _vsnprintf
#define OS_snprintf _snprintf
#endif

#define OS_Sleep(milisec) Sleep(milisec)
/*
//why this wont work I dont know..
#if (_WIN32_WINNT >= 0x0400)
#define MB_SERVICE_NOTIFICATION          0x00200000L
#else
#define MB_SERVICE_NOTIFICATION          0x00040000L
#endif
#define OS_CriticalMessage(sz) MessageBoxEx(NULL,(sz),"CriticalMessage",MB_OK|MB_ICONEXCLAMATION|MB_SYSTEMMODAL|MB_SERVICE_NOTIFICATION,MAKELANGID(LANG_NEUTRAL,SUBLANG_NEUTRAL))
*/
#define OS_CriticalMessage(sz) MessageBox(NULL,(sz),"CriticalMessage",MB_OK|MB_ICONEXCLAMATION|MB_SYSTEMMODAL)
#endif

#if defined (OS_LINUX) || defined(OS_SOLARIS) || defined(OS_OSX)
#include  <unistd.h>
#define OS_vsnprintf vsnprintf
#define OS_snprintf snprintf
#define OS_CriticalMessage(sz) fprintf(stderr,"CRITCAL ERROR:%s\n",(sz))
#define OS_Sleep(milisec) usleep(milisec * 1000)
#endif

#undef CDECL
#if defined (OS_WINDOWS) || defined(WIN32)
#define DLL_FUNC __declspec(dllexport)
#define CDECL _cdecl
#else
#define CDECL
#define DLL_FUNC
#endif

#if defined (OS_WINDOWS) || defined(WIN32)
#    define DYNLIB_HANDLE hInstance
#    define DYNLIB_LOAD( a ) LoadLibrary( a )
#    define DYNLIB_GETSYM( a, b ) GetProcAddress( a, b )
#    define DYNLIB_UNLOAD( a ) !FreeLibrary( a )

struct HINSTANCE__;
typedef struct HINSTANCE__* hInstance;

#elif defined (OS_LINUX) || defined(OS_OSX)
#    define DYNLIB_HANDLE void*
#    define DYNLIB_LOAD( a ) dlopen( a, RTLD_LAZY|RTLD_GLOBAL )
#    define DYNLIB_GETSYM( a, b ) dlsym( a, b )
#    define DYNLIB_UNLOAD( a ) dlclose( a )
/*
#elif defined(OS_OSX)
#    define DYNLIB_HANDLE CFBundleRef
#    define DYNLIB_LOAD( a ) mac_loadExeBundle( a )
#    define DYNLIB_GETSYM( a, b ) mac_getBundleSym( a, b )
#    define DYNLIB_UNLOAD( a ) mac_unloadExeBundle( a )
*/
#endif

#endif
