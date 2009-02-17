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

#if defined (OS_LINUX) 
#include <dlfcn.h>
#endif

extern void GetCurrentDir(const int buffersize, char *szDirectory);
extern void SetCurrentDir(const char *szDirectory);

extern void FindFiles(const PAL_STRING & searchstring, PAL_VECTOR<PAL_STRING> &filesFound);

#endif
