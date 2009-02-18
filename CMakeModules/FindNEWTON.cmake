# Locate Newton
# This module defines
# NEWTON_LIBRARY, NEWTON_LIBRARY_DEBUG
#NEWTON_FOUND, if false, do not try to link to Newton
# NEWTON_INCLUDE_DIR, where to find the headers
#
# NEWTON_DOUBLE_PRECISION indicates the type of lib to link against

IF(NEWTON_DOUBLE_PRECISION)
	SET(NEWTON_LIB_PATH dll_double)
ELSE(NEWTON_DOUBLE_PRECISION)
	SET(NEWTON_LIB_PATH dll)
ENDIF(NEWTON_DOUBLE_PRECISION)

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
	NAMES Newton newton32
	HINTS
	$ENV{NEWTON_DIR}
	$ENV{NEWTON_PATH}
	${ADDITIONAL_SEARCH_PATHS}
	PATH_SUFFIXES ${NEWTON_LIB_PATH} lib64 lib lib/release Library
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
	NAMES Newtond Newton_d newton32d newton32_d
	HINTS
	$ENV{NEWTON_DIR}
	$ENV{NEWTON_PATH}
	${ADDITIONAL_SEARCH_PATHS}
	PATH_SUFFIXES ${NEWTON_LIB_PATH} lib64 lib lib/debug Library
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
IF(PAL_MODULE_COPY)
	FIND_FILE(NEWTON_LIBRARY_MODULE
		NAMES "Newton${MODULE_EXT}" "newton32${MODULE_EXT}"
		HINTS
		$ENV{NEWTON_DIR}
		$ENV{NEWTON_PATH}
		${ADDITIONAL_SEARCH_PATHS}
		PATH_SUFFIXES ${NEWTON_LIB_PATH} bin
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
		NAMES "Newtond${MODULE_EXT}" "Newton_d${MODULE_EXT}" "newton32d${MODULE_EXT}" "newton32_d${MODULE_EXT}"
		HINTS
		$ENV{NEWTON_DIR}
		$ENV{NEWTON_PATH}
		${ADDITIONAL_SEARCH_PATHS}
		PATH_SUFFIXES ${NEWTON_LIB_PATH} bin
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

SET(NEWTON_FOUND "NO")
IF(NEWTON_LIBRARY AND NEWTON_INCLUDE_DIR)
  SET(NEWTON_FOUND "YES")
ENDIF(NEWTON_LIBRARY AND NEWTON_INCLUDE_DIR)

