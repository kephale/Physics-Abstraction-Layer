//(c) Adrian Boeing 2004, see liscence.txt (BSD liscence)
/**
	Abstract:
		Redefined functions for a specific OS
	Author:
		Adrian Boeing
		David Guthrie
	Revision History:
		Version 1.0 : 15/12/2009 moved inlined macros to the cpp.
*/
#include "os.h"

#if defined (OS_WINDOWS) || defined(_WIN32)

#include <windows.h>

#define OS_Sleep_DEF(milisec) Sleep(milisec)
/*
//why this wont work I dont know..
#if (_WIN32_WINNT >= 0x0400)
#define MB_SERVICE_NOTIFICATION          0x00200000L
#else
#define MB_SERVICE_NOTIFICATION          0x00040000L
#endif
#define OS_CriticalMessage(sz) MessageBoxEx(NULL,(sz),"CriticalMessage",MB_OK|MB_ICONEXCLAMATION|MB_SYSTEMMODAL|MB_SERVICE_NOTIFICATION,MAKELANGID(LANG_NEUTRAL,SUBLANG_NEUTRAL))
*/
#define OS_CriticalMessage_DEF(sz) MessageBox(NULL,(sz),"CriticalMessage",MB_OK|MB_ICONEXCLAMATION|MB_SYSTEMMODAL)

#define DYNLIB_HANDLE hInstance
#define DYNLIB_LOAD_DEF( a ) LoadLibrary( a )
#define DYNLIB_GETSYM_DEF( a, b ) GetProcAddress( a, b )
#define DYNLIB_UNLOAD_DEF( a ) !FreeLibrary( a )
struct HINSTANCE__;
typedef struct HINSTANCE__* hInstance;
#endif


#if defined (OS_LINUX) || defined(OS_OSX) || defined(__CYGWIN__)
#    include <unistd.h>
#    include <dlfcn.h>
#    define DYNLIB_HANDLE void*
#    define DYNLIB_LOAD_DEF( a ) dlopen( a, RTLD_LAZY|RTLD_GLOBAL )
#    define DYNLIB_GETSYM_DEF( a, b ) dlsym( a, b )
#    define DYNLIB_UNLOAD_DEF( a ) dlclose( a )
#    define OS_CriticalMessage_DEF(sz) fprintf(stderr,"CRITCAL ERROR:%s\n",(sz))
#    define OS_Sleep_DEF(milisec) usleep(milisec * 1000)
/*
#elif defined(OS_OSX)
#    define DYNLIB_HANDLE CFBundleRef
#    define DYNLIB_LOAD_DEF( a ) mac_loadExeBundle( a )
#    define DYNLIB_GETSYM_DEF( a, b ) mac_getBundleSym( a, b )
#    define DYNLIB_UNLOAD_DEF( a ) mac_unloadExeBundle( a )
*/
#endif

class OS_DynlibHandleClass {
public:
	DYNLIB_HANDLE m_Handle;
};


OS_DynlibHandle DYNLIB_LOAD(const char* file) {
	OS_DynlibHandle handle = new OS_DynlibHandleClass;
	handle->m_Handle = DYNLIB_LOAD_DEF(file);
	return handle;
}

void* DYNLIB_GETSYM(OS_DynlibHandle handle, const char* symbolName) {
	return (void*) DYNLIB_GETSYM_DEF(handle->m_Handle, symbolName);
}

bool DYNLIB_UNLOAD(OS_DynlibHandle handle) {
	bool result = DYNLIB_UNLOAD_DEF(handle->m_Handle) == 0;
	delete handle;
	handle = NULL;
	return result;
}

void OS_Sleep(unsigned milisec) {
	OS_Sleep_DEF(milisec);
}

void OS_CriticalMessage(const char* msg) {
	OS_CriticalMessage_DEF(msg);
}
