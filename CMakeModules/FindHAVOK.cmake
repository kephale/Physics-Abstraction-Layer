# Locate Havok
# This module defines XXX_FOUND, XXX_INCLUDE_DIRS and XXX_LIBRARIES standard variables

FIND_PATH(HAVOK_INCLUDE_DIR Common/Base/hkBase.h
	HINTS
	$ENV{HAVOK_DIR}
	$ENV{HAVOK_PATH}
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

SET(HAVOK_LIBS hkBase hkSerialize hkSceneData hkVisualize hkCompat hkpCollide hkpConstraintSolver hkpDynamics hkpInternal hkpUtilities hkpVehicle)
SET(HAVOK_LIBRARIES)

FOREACH(CUR_LIB ${HAVOK_LIBS})
	STRING(TOLOWER "${CUR_LIB}" CUR_LIB_LOWER)
	FIND_LIBRARY(HAVOK_LIBRARY_${CUR_LIB}
		NAMES ${CUR_LIB} ${CUR_LIB_LOWER}
		HINTS
			$ENV{HAVOK_DIR}
			$ENV{HAVOK_PATH}
			${ADDITIONAL_SEARCH_PATHS}
		PATH_SUFFIXES lib Lib lib64 "Lib/win32_net_8-0/debug_multithreaded_dll"
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

	FIND_LIBRARY(HAVOK_LIBRARY_${CUR_LIB}_DEBUG
		NAMES "${CUR_LIB}d" "${CUR_LIB_LOWER}d" "${CUR_LIB}_d" "${CUR_LIB_LOWER}_d"
		HINTS
			$ENV{HAVOK_DIR}
			$ENV{HAVOK_PATH}
			${ADDITIONAL_SEARCH_PATHS}
		PATH_SUFFIXES lib Lib lib64 "Lib/win32_net_8-0/release_multithreaded_dll"
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

	# Combine all libs to one variable
	IF(HAVOK_LIBRARY_${CUR_LIB})
		FIND_PACKAGE_ADD_TARGET_LIBRARIES(HAVOK "${HAVOK_LIBRARY_${CUR_LIB}}" "${HAVOK_LIBRARY_${CUR_LIB}_DEBUG}")
	ENDIF()
ENDFOREACH()


# handle the QUIETLY and REQUIRED arguments and set CURL_FOUND to TRUE if 
# all listed variables are TRUE
SET(HAVOK_LIBRARY_FULL_LIST)
FOREACH(CUR_LIB ${HAVOK_LIBS})
	LIST(APPEND HAVOK_LIBRARY_FULL_LIST "HAVOK_LIBRARY_${CUR_LIB}")
ENDFOREACH()

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(HAVOK DEFAULT_MSG ${HAVOK_LIBRARY_FULL_LIST} HAVOK_INCLUDE_DIR)

IF(HAVOK_FOUND)
	# HAVOK_LIBRARIES has been set before
	SET(HAVOK_INCLUDE_DIRS ${HAVOK_INCLUDE_DIR})
ELSE()
	SET(HAVOK_LIBRARIES)
	SET(HAVOK_INCLUDE_DIRS)
ENDIF()
