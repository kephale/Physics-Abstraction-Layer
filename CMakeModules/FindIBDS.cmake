# Locate IBDS
# This module defines XXX_FOUND, XXX_INCLUDE_DIRS and XXX_LIBRARIES standard variables

FIND_PATH(IBDS_INCLUDE_DIR
	NAMES "DynamicSimulation/Simulation.h" "Math/SimMath.h"
	HINTS
	$ENV{IBDS_DIR}
	$ENV{IBDS_PATH}
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

FIND_LIBRARY(IBDS_LIBRARY
	NAMES ibds
	HINTS
	$ENV{IBDS_DIR}
	$ENV{IBDS_PATH}
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

FIND_LIBRARY(IBDS_LIBRARY_DEBUG 
	NAMES ibdsd ibds_d
	HINTS
	$ENV{IBDS_DIR}
	$ENV{IBDS_PATH}
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


# handle the QUIETLY and REQUIRED arguments and set CURL_FOUND to TRUE if 
# all listed variables are TRUE
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(IBDS DEFAULT_MSG IBDS_LIBRARY IBDS_INCLUDE_DIR)

FIND_PACKAGE_SET_STD_INCLUDE_AND_LIBS(IBDS)

