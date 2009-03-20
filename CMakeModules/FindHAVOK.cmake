# Locate Havok
# This module defines
# HAVOK_LIBRARY, HAVOK_LIBRARY_DEBUG
#HAVOK_FOUND, if false, do not try to link to Havok
# HAVOK_INCLUDE_DIR, where to find the headers

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

	# Combine all libs to two variables
	IF(HAVOK_LIBRARY_${CUR_LIB} AND NOT HAVOK_LIBRARY_ERROR)
		SET(HAVOK_LIBRARY "${HAVOK_LIBRARY};${HAVOK_LIBRARY_${CUR_LIB}}")
	ELSE()
		SET(HAVOK_LIBRARY "HAVOK_LIBRARY-NOTFOUND")
		SET(HAVOK_LIBRARY_ERROR "YES")
	ENDIF()

	IF(HAVOK_LIBRARY_${CUR_LIB}_DEBUG AND NOT HAVOK_LIBRARY_DEBUG_ERROR)
		SET(HAVOK_LIBRARY_DEBUG "${HAVOK_LIBRARY_DEBUG};${HAVOK_LIBRARY_${CUR_LIB}_DEBUG}")
	ELSE()
		SET(HAVOK_LIBRARY_DEBUG "HAVOK_LIBRARY_DEBUG-NOTFOUND")
		SET(HAVOK_LIBRARY_DEBUG_ERROR "YES")
	ENDIF()
ENDFOREACH()


SET(HAVOK_FOUND "NO")
IF(HAVOK_LIBRARY AND HAVOK_INCLUDE_DIR)
  SET(HAVOK_FOUND "YES")
ENDIF()
