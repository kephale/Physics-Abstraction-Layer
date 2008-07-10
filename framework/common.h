#ifndef COMMON_H
#define COMMON_H
//(c) Adrian Boeing 2004, see liscence.txt (BSD liscence)
/**
	Abstract:
		A set of common defined and portable items
	Author:
		Adrian Boeing
	Revision History:
		Version 3.0.2:28/11/06 SunPro C++ support
		Version 3.0.1:05/12/04 MSVC8 support
		Version 3.0  :19/08/04 PAL Cut
		Version 2.9.3:25/03/04 RTTI gcc default hack
		Version 2.9.2:03/02/04 GCC 2.95 support
		Version 2.9.1:22/01/04 VC6-Uni hack
		Version 2.9 : 20/11/03 VC.NET fix
		Version 2.8 : 18/11/03 Add named line_var support & complete function macros
		Version 2.7 : 18/11/03 Improved function support & added stringizer&line_var support & transparent windows header support
		Version 2.6 : 15/11/03 Added OS support, and function/elipsis macros
		Version 2.5 : 14/11/03 Added compiler specific defines, some DMC support
		Version 2.4 : 13/11/03 RTTI Compiler Support
		Version 2.3 : 08/11/03 Endian, Signed Int's
		Version 2.2 : 01/11/03 Cleaner Namespace
		Version 2.1 : 06/10/03 Intel Compiler Support
		Version 2.0 : 29/09/03 Initial complete rewrite
	TODO:
		-Auto configure /Zi for VC
*/

#define COMMON_VERSION 2.9

//disable some annoying MSC warnings
#if defined(_MSC_VER)
#pragma warning( disable : 4503 ) // warning: decorated name length exceeded
#if _MSC_VER < 1300
#pragma warning( disable : 4786 ) // ident trunc to '255' chars in debug info
#endif
#endif

//so I can redefine these later, if neccessary
#include <string>
#include <vector>
#include <list>

typedef std::string STRING;
#define VECTOR std::vector
#define LIST std::list


//so I can redefine these later, if neccessary (non-stl)
#if defined(__cplusplus)

#if defined(WIN32)
typedef int BOOL;
#ifndef FALSE
#define FALSE               0
#endif
#ifndef TRUE
#define TRUE                1
#endif
#else
typedef bool BOOL;
# ifndef TRUE
# define TRUE true
# endif
# ifndef FALSE
# define FALSE false
#endif //win32
#endif //__cplusplus
#else
#define BOOL int
#define TRUE 1
#define FALSE 0
#endif
typedef unsigned int UINT;
typedef int INT;
typedef float FLOAT;

//thats it! im giving up! compiler specific macros follow:
//microsoft:
#if defined(_MSC_VER)
#define MICROSOFT_VC

#if _MSC_VER >= 1400
#define MICROSOFT_VC_8 _MSC_VER
#elif _MSC_VER >= 1300
#define MICROSOFT_VC_7 _MSC_VER
#elif _MSC_VER >= 1200
#define MICROSOFT_VC_6 _MSC_VER
#endif //MSCVER

#endif //MSVC
//gcc:
#if defined(__GNUC__)
#define GNU_C
#endif
//intel:
#if defined(__INTEL_COMPILER)
#define INTEL_C
#endif
//digitalmars:
#if defined (__DMC__)
#define DIGITALMARS_C
#endif
//sun
#if defined(__SUNPRO_C) || defined(__SUNPRO_CC)
#define SUN_C
#endif


//i dont use these yet, there here for other people, or when i do (if ever) finally get a copy of them:
#if defined (__MWERKS__)
#define METROWORKS_C
#endif
#if defined (__BORLANDC__)
#define BORLAND_C
#endif
#if defined (__WATCOMC__)
#define WATCOM_C
#endif
#if defined (__COMO__)
#define COMO_C
#endif

//os specific ifdefs -- not sure if these are correct!
#undef OS_WINDOWS
#undef OS_LINUX

#if defined (WIN32)
#define OS_WINDOWS
#endif
#if defined(linux) || defined(__linux) || defined(__linux__)
#define OS_LINUX
#endif
#if defined (sun) || defined(__sun) || defined(__SVR4) || defined(__svr4__)
#define OS_SOLARIS
#endif
#if defined(__FreeBSD__) || defined(__NetBSD__) || defined(__OpenBSD__)
#endif
#if defined(__BEOS__)
#endif
#if defined (MAC_OS_X)
#define OS_OSX
#endif
#if defined(macintosh) || defined(__APPLE__) || defined(__APPLE_CC__)
#define OS_OSX
#endif
#if defined(__amigaos__)
#endif
#if defined(unix) || defined(__unix) || defined(_XOPEN_SOURCE) || defined(_POSIX_SOURCE)
#endif
//defined(__sgi)
//irix

//cpu specific - not sure if these are correct!
//_M_M68K, for GCC

#if defined(_MSC_VER) || defined(__LCC__) || defined(__INTEL_COMPILER)

#if defined(WIN32)
typedef unsigned long      DWORD;
typedef unsigned char      BYTE;
typedef unsigned short     WORD;
typedef unsigned __int64   QWORD;

#include <basetsd.h>

#if defined(MICROSOFT_VC_6)
typedef unsigned char    UINT8;
typedef signed short     INT16;
#else
typedef unsigned __int8  UINT8;
typedef signed __int16   INT16;
#endif

#else
typedef unsigned __int8    BYTE;
typedef unsigned __int16   WORD;
typedef unsigned __int32   DWORD;
typedef unsigned __int64   QWORD;
typedef signed __int8    INT8;
typedef signed __int16   INT16;
typedef signed __int32	 INT32;
typedef signed __int64	 INT64;
typedef unsigned __int8  UINT8;
typedef unsigned __int32 UINT32;
#endif

#elif defined(__GNUC__) || defined(__MWERKS__) || defined(__SUNPRO_C) || defined(__DMC__)

typedef unsigned char BYTE;
typedef unsigned short WORD;
typedef unsigned long DWORD;
typedef unsigned long long QWORD;

typedef   signed char		INT8;
typedef unsigned char		UINT8;
typedef   signed short		INT16;
typedef   signed int		INT32;
typedef   signed long long	INT64;
typedef unsigned int		UINT32;

#else

//#error Dont know word sizes for these machines/compilers
typedef unsigned char BYTE;
typedef unsigned short WORD;
typedef unsigned int DWORD;
typedef struct {DWORD a, b;} QWORD;

//im just guessing here!
typedef unsigned char		UINT8;
typedef   signed char		INT8;
typedef   signed short		INT16;
typedef   signed int		INT32;
typedef unsigned int		UINT32;
#endif

//Endian code comes from SDL
#if defined(OS_LINUX)
#include <endian.h>
#elif defined(OS_OSX)
#include <machine/endian.h>
#else
#define LIL_ENDIAN	1234
#define BIG_ENDIAN	4321
#endif
#if  defined(__i386__) || defined(__ia64__) || defined(WIN32) || \
    (defined(__alpha__) || defined(__alpha)) || \
     defined(__arm__) || \
    (defined(__mips__) && defined(__MIPSEL__)) || \
     defined(__SYMBIAN32__) || \
     defined(__LITTLE_ENDIAN__)
#define BYTEORDER	LIL_ENDIAN
#else
#define BYTEORDER	BIG_ENDIAN
#endif

//compiled with RTTI?
#ifndef RTTI_ENABLED
#if defined(__RTTI)
#define RTTI_ENABLED 1
#elif defined(_CPPRTTI)
#define RTTI_ENABLED 1
#elif defined (GNU_C)
#define RTTI_ENABLED 1 //im just assuming this here
#else
//no RTTI as far as I know?
#undef RTTI_ENABLED
#endif
#endif

//can we use __VA_ARGS__ ?
#if defined(GNU_C) || defined(DIGITALMARS_C) || defined (INTEL_C)
#define MACRO_ELLIPSIS
//stable debian sucks
# if defined (GNU_C)
#  if __GNUC__ < 3
#  undef MACRO_ELLIPSIS
#  endif
# endif
#else
#undef MACRO_ELLIPSIS
#endif

//can we use __FUNCTION__ or similar?
#if defined (MICROSOFT_VC_7) || defined(DIGITALMARS_C) || defined(GNU_C)
#define MACRO_FUNCTION
#else
#undef MACRO_FUNCTION
#endif
//c99:__func__
// -- not sure wh


//stringizing function
#define STRINGIZE(something) STRINGIZE_HELPER(something)
#define STRINGIZE_HELPER(something) #something
//line-dependent (read: unique) variable generation
#define LINE_VAR _LINE_VAR(__LINE__)
#define _LINE_VAR(l) __LINE_VAR(l)
#define __LINE_VAR(l) lineVar ## l
//line-dependent (read: unique) variable generation with a name
#define NAMED_LINE_VAR(name) _NAMED_LINE_VAR(name,__LINE__)
#define _NAMED_LINE_VAR(name,l) __NAMED_LINE_VAR(name,l)
#define __NAMED_LINE_VAR(name,l) name ## l

#if defined (MICROSOFT_VC)
//#pragma message( "STRINGIZE and LINE_VAR wont work unless edit and continue is disabled in the project (use /Zi, not /ZI)" )
//#pragma comment(settheoption, "/Zi")
#endif

//really only neccessary for MSVC
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
#ifndef M_PIf
#define M_PIf 3.14159265358979323846f
#endif

#endif
