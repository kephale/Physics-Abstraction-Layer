# Locate Newton
# This module defines
# NEWTON_LIBRARY, NEWTON_LIBRARY_DEBUG
#NEWTON_FOUND, if false, do not try to link to Newton
# NEWTON_INCLUDE_DIR, where to find the headers

FIND_PATH(NEWTON_INCLUDE_DIR Newton.h
	HINTS
	$ENV{NEWTON_DIR}
	$ENV{NEWTON_PATH}
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

FIND_LIBRARY(NEWTON_LIBRARY
	NAMES newton32
	HINTS
	$ENV{NEWTON_DIR}
	$ENV{NEWTON_PATH}
	${ADDITIONAL_SEARCH_PATHS}
	PATH_SUFFIXES lib64 lib lib/release Library dll
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

FIND_LIBRARY(NEWTON_LIBRARY_DEBUG 
	NAMES newton32d newton32_d
	HINTS
	$ENV{NEWTON_DIR}
	$ENV{NEWTON_PATH}
	${ADDITIONAL_SEARCH_PATHS}
	PATH_SUFFIXES lib64 lib lib/debug Library dll
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
	FIND_FILE(NEWTON_LIBRARY_MODULE
		NAMES Newton.dll
		HINTS
		$ENV{NEWTON_DIR}
		$ENV{NEWTON_PATH}
		${ADDITIONAL_SEARCH_PATHS}
		PATH_SUFFIXES bin dll
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

	FIND_FILE(NEWTON_LIBRARY_MODULE_DEBUG 
		NAMES Newtond.dll Newton_d.dll
		HINTS
		$ENV{NEWTON_DIR}
		$ENV{NEWTON_PATH}
		${ADDITIONAL_SEARCH_PATHS}
		PATH_SUFFIXES bin dll
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

SET(NEWTON_FOUND "NO")
IF(NEWTON_LIBRARY AND NEWTON_INCLUDE_DIR)
  SET(NEWTON_FOUND "YES")
ENDIF(NEWTON_LIBRARY AND NEWTON_INCLUDE_DIR)
