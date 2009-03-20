# Locate IBDS
# This module defines
# IBDS_LIBRARY, IBDS_LIBRARY_DEBUG
#IBDS_FOUND, if false, do not try to link to IBDS
# IBDS_INCLUDE_DIR, where to find the headers

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

SET(IBDS_FOUND "NO")
IF(IBDS_LIBRARY AND IBDS_INCLUDE_DIR)
  SET(IBDS_FOUND "YES")
ENDIF()

