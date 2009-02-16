# Locate SPE
# This module defines
# SPE_LIBRARY, SPE_LIBRARY_DEBUG
#SPE_FOUND, if false, do not try to link to SPE
# SPE_INCLUDE_DIR, where to find the headers

FIND_PATH(SPE_INCLUDE_DIR SPE.h
	HINTS
	$ENV{SPE_DIR}
	$ENV{SPE_PATH}
	${ADDITIONAL_SEARCH_PATHS}
	PATH_SUFFIXES include
	PATHS
		~/Library/Frameworks
		/Library/Frameworks
		/usr/local
		/usr
		/sw # Fink
		/opt/local # DarwinPorts
		/opt/csw # Blastwave
		/opt
)

FIND_LIBRARY(SPE_LIBRARY
	NAMES SPE
	HINTS
	$ENV{SPE_DIR}
	$ENV{SPE_PATH}
	${ADDITIONAL_SEARCH_PATHS}
	PATH_SUFFIXES lib64 lib lib/release Library
	PATHS
		~/Library/Frameworks
		/Library/Frameworks
		/usr/local
		/usr
		/sw
		/opt/local
		/opt/csw
		/opt
)

FIND_LIBRARY(SPE_LIBRARY_DEBUG 
	NAMES SPEd SPE_d
	HINTS
	$ENV{SPE_DIR}
	$ENV{SPE_PATH}
	${ADDITIONAL_SEARCH_PATHS}
	PATH_SUFFIXES lib64 lib lib/debug Library
	PATHS
		~/Library/Frameworks
		/Library/Frameworks
		/usr/local
		/usr
		/sw
		/opt/local
		/opt/csw
		/opt
)


# Special for DLL copy
IF(WIN32)
	FIND_FILE(SPE_LIBRARY_MODULE
		NAMES SPE.dll
		HINTS
		$ENV{SPE_DIR}
		$ENV{SPE_PATH}
		${ADDITIONAL_SEARCH_PATHS}
		PATH_SUFFIXES bin
		DOC "Optional path of the release DLL, to be copied after the build."
		PATHS
			~/Library/Frameworks
			/Library/Frameworks
			/usr/local
			/usr
			/sw
			/opt/local
			/opt/csw
			/opt
	)

	FIND_FILE(SPE_LIBRARY_MODULE_DEBUG 
		NAMES SPEd.dll SPE_d.dll
		HINTS
		$ENV{SPE_DIR}
		$ENV{SPE_PATH}
		${ADDITIONAL_SEARCH_PATHS}
		PATH_SUFFIXES bin
		DOC "Optional path of the debug DLL, to be copied after the build."
		PATHS
			~/Library/Frameworks
			/Library/Frameworks
			/usr/local
			/usr
			/sw
			/opt/local
			/opt/csw
			/opt
	)
ENDIF(WIN32)

SET(SPE_FOUND "NO")
IF(SPE_LIBRARY AND SPE_INCLUDE_DIR)
  SET(SPE_FOUND "YES")
ENDIF(SPE_LIBRARY AND SPE_INCLUDE_DIR)

