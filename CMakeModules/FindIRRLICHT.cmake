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
	# Irrlicht needs the jpeg and png libraries to be linked
	# separately (at least on some Unices).  On some Unices
	# Irrlicht might use XFree86-VidModeExtension.
	IF ( UNIX AND NOT APPLE )
		find_library( X11_VIDEO_LIBRARY Xxf86vm
			/usr/lib64
			/usr/lib
			/usr/local/lib64
			/usr/local/lib
			/sw/lib
			/opt/local/lib
			DOC "Xxf86vm"
			)

		if( X11_VIDEO_LIBRARY )
			message( "--- Looking for Xxf86vm - found" )
			message( "--- Libs: " ${X11_VIDEO_LIBRARY} )
			add_definitions( -D_IRR_LINUX_X11_VIDMODE_ )              
		else( X11_VIDEO_LIBRARY )
			message( "--- Looking for Xxf86vm - not found" )
		endif( X11_VIDEO_LIBRARY )

		find_library( JPEG_LIBRARY jpeg
			/usr/lib64
			/usr/lib
			/usr/local/lib64
			/usr/local/lib
			/sw/lib
			/opt/local/lib
			DOC "jpeg"
			)

		if( JPEG_LIBRARY )
			message( "--- Looking for JPEG library - found" )
			message( "--- Libs: " ${JPEG_LIBRARY} )
		else( JPEG_LIBRARY )
			message( "--- Looking for JPEG library - not found" )
		endif( JPEG_LIBRARY )

		find_library( PNG_LIBRARY png
			/usr/lib64
			/usr/lib
			/usr/local/lib64
			/usr/local/lib
			/sw/lib
			/opt/local/lib
			DOC "png"
			)

		if( PNG_LIBRARY )
			message( "--- Looking for PNG library - found" )
			message( "--- Libs: " ${PNG_LIBRARY} )
		else( PNG_LIBRARY )
			message( "--- Looking for PNG library - not found" )
		endif( PNG_LIBRARY )

		set( IRRLICHT_EXTRA_LIBRARIES ${X11_LIBRARIES} ${X11_VIDEO_LIBRARY} ${JPEG_LIBRARY} ${PNG_LIBRARY} )

	ENDIF (UNIX AND NOT APPLE)
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
set(IRRLICHT_LIBRARIES
	${IRRLICHT_LIBRARIES}
	${IRRLICHT_EXTRA_LIBRARIES}
	)
message("*** FindIRRLICHT: final IRRLICHT_LIBRARIES=${IRRLICHT_LIBRARIES}")
