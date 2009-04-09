# Locate IBDS
# This module defines XXX_FOUND, XXX_INCLUDE_DIRS and XXX_LIBRARIES standard variables
INCLUDE(FindPackageTargetLibraries)

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

SET(IBDS_LIBS "CollisionDetection" "DynamicSimulation" "Math" "libBulletCollision" "libLinearMath" "qhull")		# Tested with  IBDS 1.09
SET(IBDS_LIBRARIES)

FOREACH(CUR_LIB ${IBDS_LIBS})
	STRING(TOLOWER "${CUR_LIB}" CUR_LIB_LOWER)
	FIND_LIBRARY(IBDS_LIBRARY_${CUR_LIB}
		NAMES "Lib${CUR_LIB}" "lib${CUR_LIB_LOWER}" ${CUR_LIB} ${CUR_LIB_LOWER}
		HINTS
			$ENV{IBDS_DIR}
			$ENV{IBDS_PATH}
			${ADDITIONAL_SEARCH_PATHS}
		PATH_SUFFIXES lib64 lib src lib/release
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

	FIND_LIBRARY(IBDS_LIBRARY_${CUR_LIB}_DEBUG
		NAMES "Lib${CUR_LIB}d" "lib${CUR_LIB_LOWER}d" "${CUR_LIB}d" "${CUR_LIB_LOWER}d" "Lib${CUR_LIB}_d" "lib${CUR_LIB_LOWER}_d" "${CUR_LIB}_d" "${CUR_LIB_LOWER}_d"
		HINTS
			$ENV{IBDS_DIR}
			$ENV{IBDS_PATH}
			${ADDITIONAL_SEARCH_PATHS}
		PATH_SUFFIXES lib64 lib src lib/debug
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
	
	IF(IBDS_LIBRARY_${CUR_LIB})
		FIND_PACKAGE_ADD_TARGET_LIBRARIES(IBDS "${IBDS_LIBRARY_${CUR_LIB}}" "${IBDS_LIBRARY_${CUR_LIB}_DEBUG}")
	ENDIF()
ENDFOREACH()

# handle the QUIETLY and REQUIRED arguments and set CURL_FOUND to TRUE if 
# all listed variables are TRUE
SET(IBDS_LIBRARY_FULL_LIST)
FOREACH(CUR_LIB ${IBDS_LIBS})
	LIST(APPEND IBDS_LIBRARY_FULL_LIST "IBDS_LIBRARY_${CUR_LIB}")
ENDFOREACH()

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(IBDS DEFAULT_MSG ${IBDS_LIBRARY_FULL_LIST} IBDS_INCLUDE_DIR)

IF(IBDS_FOUND)
	SET(IBDS_INCLUDE_DIRS ${IBDS_INCLUDE_DIR})
ELSE()
	SET(IBDS_INCLUDE_DIRS)
ENDIF()
