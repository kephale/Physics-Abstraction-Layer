# Locate Jiggle
# This module defines XXX_FOUND, XXX_INCLUDE_DIRS and XXX_LIBRARIES standard variables

FIND_PATH(JIGGLE_INCLUDE_DIR jiglib.hpp
	HINTS
	$ENV{JIGGLE_DIR}
	$ENV{JIGGLE_PATH}
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

FIND_LIBRARY(JIGGLE_LIBRARY
	NAMES JigLib JigLibRelease
	HINTS
	$ENV{JIGGLE_DIR}
	$ENV{JIGGLE_PATH}
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

FIND_LIBRARY(JIGGLE_LIBRARY_DEBUG 
	NAMES JigLibd JigLibDebug
	HINTS
	$ENV{JIGGLE_DIR}
	$ENV{JIGGLE_PATH}
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


# handle the QUIETLY and REQUIRED arguments and set CURL_FOUND to TRUE if 
# all listed variables are TRUE
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(JIGGLE DEFAULT_MSG JIGGLE_LIBRARY JIGGLE_INCLUDE_DIR)
INCLUDE(FindPackageTargetLibraries)
FIND_PACKAGE_SET_STD_INCLUDE_AND_LIBS(JIGGLE)
