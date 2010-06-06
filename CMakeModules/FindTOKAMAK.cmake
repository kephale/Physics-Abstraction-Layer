# Locate Tokamak
# This module defines XXX_FOUND, XXX_INCLUDE_DIRS and XXX_LIBRARIES standard variables

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
	NAMES tokamakdll tokamak tokamak_md
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
	NAMES tokamakdlld tokamakdll_d tokamak_d
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
ENDIF()


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
ENDIF()



# handle the QUIETLY and REQUIRED arguments and set CURL_FOUND to TRUE if 
# all listed variables are TRUE
SET(TOKAMAK_INCLUDE_DIR_FULL_LIST ${TOKAMAK_INCLUDE_DIR})
IF(TOKAMAK_USE_QHULL)
	LIST(APPEND TOKAMAK_INCLUDE_DIR_FULL_LIST ${TOKAMAK_QHULL_INCLUDE_DIR})
ENDIF()

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(TOKAMAK DEFAULT_MSG TOKAMAK_LIBRARY TOKAMAK_INCLUDE_DIR_FULL_LIST)

IF(TOKAMAK_FOUND)
	FIND_PACKAGE_SET_TARGET_LIBRARIES(TOKAMAK)
	SET(TOKAMAK_INCLUDE_DIRS ${TOKAMAK_INCLUDE_DIR_FULL_LIST})
ELSE()
	SET(TOKAMAK_LIBRARIES)
	SET(TOKAMAK_INCLUDE_DIRS)
ENDIF()
