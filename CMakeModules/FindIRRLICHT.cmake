# - Try to find Irrlicht
# Once done this will define
#
# IRRLICHT_FOUND - system has Irrlicht
# IRRLICHT_INCLUDE_DIRS - the Irrlicht include directory
# IRRLICHT_LIBRARIES - Link these to use Irrlicht
# IRRLICHT_DEFINITIONS - Compiler switches required for using Irrlicht
#
# Copyright (c) 2008 Olof Naessen <olof.naessen@gmail.com>
#
# Redistribution and use is allowed according to the terms of the New
# BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.
#

if (IRRLICHT_LIBRARIES AND IRRLICHT_INCLUDE_DIRS)
	# in cache already
	set(IRRLICHT_FOUND TRUE)
else (IRRLICHT_LIBRARIES AND IRRLICHT_INCLUDE_DIRS)
	find_path(IRRLICHT_INCLUDE_DIR
	NAMES
		irrlicht.h
	PATHS
		/usr/include
		/usr/local/include
		/opt/local/include
		~/Library/Frameworks
		/Library/Frameworks
		/usr/local
		/usr
		/sw/ 		# Fink
		/opt/local 	# DarwinPorts
		/opt/csw 	# Blastwave
		/opt
		/lib
		/lib/Irrlicht-1.6/include
		/lib/Irrlicht-1.6.1/include
	PATH_SUFFIXES
		irrlicht
)

find_library(IRRLICHT_LIBRARY
	NAMES
		irrlicht
	HINTS
		$ENV{IRRLICHT_DIR}
		$ENV{IRRLICHT_PATH}
		${ADDITIONAL_SEARCH_PATHS}
	PATH_SUFFIXES 
		lib64
		lib
		"source/Irrlicht/MacOSX/build/Release"	# Mac
		"lib/Win32-visualstudio"				# Windows
		"lib/Win64-visualstudio"				# Windows
		"lib/Win32-gcc"							# Windows
		"lib/Linux"								# Linux
	PATHS
		/usr/lib
		/usr/local/lib
		/opt/local/lib
		/lib
)

set(IRRLICHT_INCLUDE_DIRS
	${IRRLICHT_INCLUDE_DIR}
)
set(IRRLICHT_LIBRARIES
	${IRRLICHT_LIBRARY}
)

if (IRRLICHT_INCLUDE_DIRS AND IRRLICHT_LIBRARIES)
	set(IRRLICHT_FOUND TRUE)
endif (IRRLICHT_INCLUDE_DIRS AND IRRLICHT_LIBRARIES)

	if (IRRLICHT_FOUND)
		if (NOT Irrlicht_FIND_QUIETLY)
			message(STATUS "Found Irrlicht: include dirs='${IRRLICHT_INCLUDE_DIRS}', libs='${IRRLICHT_LIBRARIES}'")
		endif (NOT Irrlicht_FIND_QUIETLY)
	else (IRRLICHT_FOUND)
		if (Irrlicht_FIND_REQUIRED)
			message(FATAL_ERROR "Could not find Irrlicht")
		endif (Irrlicht_FIND_REQUIRED)
	endif (IRRLICHT_FOUND)

	# show the IRRLICHT_INCLUDE_DIRS and IRRLICHT_LIBRARIES variables only in the advanced view
	mark_as_advanced(IRRLICHT_INCLUDE_DIRS IRRLICHT_LIBRARIES)
	
endif (IRRLICHT_LIBRARIES AND IRRLICHT_INCLUDE_DIRS)

# handle the QUIETLY and REQUIRED arguments and set CURL_FOUND to TRUE if 
# all listed variables are TRUE
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(IRRLICHT DEFAULT_MSG IRRLICHT_LIBRARY IRRLICHT_INCLUDE_DIR)
INCLUDE(FindPackageTargetLibraries)
FIND_PACKAGE_SET_STD_INCLUDE_AND_LIBS(IRRLICHT)
