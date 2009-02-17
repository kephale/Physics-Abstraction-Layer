#include "osfs.h"
#include "errorlog.h"
//(c) Adrian Boeing 2007, see liscence.txt (BSD liscence)
/*
	Abstract:
		A set of OS independent file system operations
	Author: 
		Adrian Boeing
	Revision History:
		Version 1.0.1:05/01/07 VC6 fix
		Version 1.0  :06/12/07 Initial release
	TODO:
*/

#if defined (OS_WINDOWS) || defined(WIN32)
#include <windows.h>
#include <direct.h>
#include <stdio.h>
#include <stdlib.h>
#include <io.h>

#if defined (MICROSOFT_VC_6)
typedef int	intptr_t;
#endif

void GetCurrentDir(const int buffersize, char *szDirectory) {
	GetCurrentDirectory(buffersize,szDirectory);
}

void SetCurrentDir(const char *szDirectory) {
	SetCurrentDirectory(szDirectory);
}

void FindFiles(const PAL_STRING & searchString, PAL_VECTOR<PAL_STRING> &filesFound) {
	PAL_STRING::size_type sepPos = searchString.find_last_of(".");
	if(sepPos == -1 || sepPos == 0)
	{
		STATIC_SET_ERROR("Invalid search string: %s",searchString.c_str());
		return;
	}

	// search string should now be: *.extension
	PAL_STRING internalSearchString = searchString.substr(sepPos, searchString.length());
	// and now only the point with extension. 


	struct _finddata_t fileinfo;

	// findfirst() needs a regular expression of this type: *.extension
	// build this from the search string. Try to get a first file
	intptr_t handle = _findfirst( (PAL_STRING("*")+internalSearchString).c_str(), &fileinfo);

	// was at least one file found?
	if(handle != -1)
	{
		// yes...
		filesFound.push_back(fileinfo.name);
		while(true)
		{
			if(_findnext(handle, &fileinfo) != 0)
				break;
			filesFound.push_back(fileinfo.name);
		}
	}
}
#elif defined (OS_LINUX) || defined(OS_OSX)

#include <dirent.h>
#include <dlfcn.h>

void GetCurrentDir(const int buffersize, char *szDirectory) {
	getcwd(szDirectory, buffersize);
}

void SetCurrentDir(const char *szDirectory) {
	chdir(szDirectory);
}

void FindFiles(const PAL_STRING & searchString, PAL_VECTOR<PAL_STRING> &filesFound) {
	PAL_STRING::size_type sepPos = searchString.find_last_of(".");
	if(sepPos == -1 || sepPos == 0)
	{
		STATIC_SET_ERROR("Invalid search string: %s",searchString.c_str());
		return;
	}

	// search string should now be: *.extension
	PAL_STRING internalSearchString = searchString.substr(sepPos, searchString.length());
	// and now only the point with extension. 

	DIR * dir = opendir (".");
	if (dir == NULL){
		STATIC_SET_ERROR("Could not open directory\n");
		return;
	}

	struct dirent *pDir=NULL;
	while((pDir = readdir(dir)) != NULL)
	{
		// check if the file has the correct extension
		PAL_STRING filename = (*pDir).d_name;
		PAL_STRING::size_type nameSepPos = filename.find_last_of(".");
		if (nameSepPos != PAL_STRING::npos) 
		{
			// The file name has an extension, look at it.
			PAL_STRING extension = filename.substr(nameSepPos, filename.length());

			// if both have the same file extension, add them to the list
			if(extension == internalSearchString)
			{
				filesFound.push_back(filename);
			}
		} // TODO: else?
	}
	closedir(dir);
}
#endif
