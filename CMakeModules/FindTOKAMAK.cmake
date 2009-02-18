# Locate Tokamak
# This module defines
# TOKAMAK_LIBRARY, TOKAMAK_LIBRARY_DEBUG
#TOKAMAK_FOUND, if false, do not try to link to Tokamak
# TOKAMAK_INCLUDE_DIR, where to find the headers

FIND_PATH(TOKAMAK_INCLUDE_DIR tokamak.h
	HINTS
	$ENV{TOKAMAK_DIR}
	$ENV{TOKAMAK_PATH}
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

FIND_LIBRARY(TOKAMAK_LIBRARY
	NAMES tokamakdll
	HINTS
	$ENV{TOKAMAK_DIR}
	$ENV{TOKAMAK_PATH}
	${ADDITIONAL_SEARCH_PATHS}
	PATH_SUFFIXES lib64 lib lib/release
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

FIND_LIBRARY(TOKAMAK_LIBRARY_DEBUG 
	NAMES tokamakdlld tokamakdll_d
	HINTS
	$ENV{TOKAMAK_DIR}
	$ENV{TOKAMAK_PATH}
	${ADDITIONAL_SEARCH_PATHS}
	PATH_SUFFIXES lib64 lib lib/debug
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

# DLL/so copy
IF(PAL_MODULE_COPY)
	FIND_FILE(TOKAMAK_LIBRARY_MODULE
		NAMES "tokamakdll${MODULE_EXT}" "tokamak${MODULE_EXT}"
		HINTS
		$ENV{TOKAMAK_DIR}
		$ENV{TOKAMAK_PATH}
		${ADDITIONAL_SEARCH_PATHS}
		PATH_SUFFIXES lib64 lib lib/release bin dll
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

	FIND_FILE(TOKAMAK_LIBRARY_MODULE_DEBUG 
		NAMES "tokamakdlld${MODULE_EXT}" "tokamakdll_d${MODULE_EXT}" "tokamakd${MODULE_EXT}" "tokamak_d${MODULE_EXT}"
		HINTS
		$ENV{TOKAMAK_DIR}
		$ENV{TOKAMAK_PATH}
		${ADDITIONAL_SEARCH_PATHS}
		PATH_SUFFIXES lib64 lib lib/debug bin dll
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
ENDIF(PAL_MODULE_COPY)


IF(TOKAMAK_USE_QHULL)
	FIND_PATH(TOKAMAK_QHULL_INCLUDE_DIR qhull.h
		HINTS
		$ENV{TOKAMAK_DIR}
		$ENV{TOKAMAK_PATH}
		${ADDITIONAL_SEARCH_PATHS}
		PATH_SUFFIXES include qhull/src
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
ENDIF(TOKAMAK_USE_QHULL)


SET(TOKAMAK_FOUND "NO")
IF(TOKAMAK_LIBRARY AND TOKAMAK_INCLUDE_DIR)
  SET(TOKAMAK_FOUND "YES")
ENDIF(TOKAMAK_LIBRARY AND TOKAMAK_INCLUDE_DIR)

