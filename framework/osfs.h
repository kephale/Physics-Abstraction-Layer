#ifndef OSFS_H
#define OSFS_H
//(c) Adrian Boeing 2007, see liscence.txt (BSD liscence)
/**
	Abstract:
		A set of OS independent file system operations
	Author: 
		Adrian Boeing
	Revision History:
		Version 1.0 : 06/12/07 Initial release
	TODO:
*/
#include "common.h"
#include "pal/palException.h"

#ifdef _WIN32
#define PATH_SEPARATOR_CHAR ';'
#elif defined(__linux) || defined(__APPLE__) || defined(__CYGWIN__)
#define PATH_SEPARATOR_CHAR ':'
#else
#warning "Unknown operating system. May not work properly."
#define PATH_SEPARATOR_CHAR ':'
#endif

extern void GetCurrentDir(const int buffersize, char *szDirectory);
extern void SetCurrentDir(const char *szDirectory);

extern void FindFiles(PAL_STRING searchstring, PAL_VECTOR<PAL_STRING> &filesFound);

#endif
