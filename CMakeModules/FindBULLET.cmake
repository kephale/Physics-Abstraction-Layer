# Locate Bullet
# This module defines
# BULLET_LIBRARY, BULLET_LIBRARY_DEBUG (and sub-variables)
# BULLET_FOUND, if false, do not try to link to Bullet
# BULLET_INCLUDE_DIR, where to find the headers
#
# Define BULLET_SINGLE_THREADED to "YES" to search for single threaded variant.

SET(BULLET_MSVC_LIB_DIR "")
IF(CMAKE_GENERATOR MATCHES "Visual Studio")
	IF (${CMAKE_GENERATOR} MATCHES "2008")
		SET(BULLET_MSVC_LIB_DIR "9")
	ELSEIF (${CMAKE_GENERATOR} MATCHES "2005")
		SET(BULLET_MSVC_LIB_DIR "8")
	ELSEIF (${CMAKE_GENERATOR} MATCHES "2003")
		SET(BULLET_MSVC_LIB_DIR "71")
	ELSEIF (${CMAKE_GENERATOR} MATCHES "2002")
		SET(BULLET_MSVC_LIB_DIR "7")
	ELSEIF (${CMAKE_GENERATOR} MATCHES "6")
		SET(BULLET_MSVC_LIB_DIR "6")
	ENDIF (${CMAKE_GENERATOR} MATCHES "2008")
ENDIF()

FIND_PATH(BULLET_INCLUDE_DIR btBulletDynamicsCommon.h
	HINTS
	$ENV{BULLET_DIR}
	$ENV{BULLET_PATH}
	${ADDITIONAL_SEARCH_PATHS}
	PATH_SUFFIXES include src
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

IF(BULLET_SINGLE_THREADED)
	#SET(BULLET_LIBS "BulletDynamics" "BulletCollision" "BulletSoftBody" "LinearMath" "BulletMultiThreaded")
	SET(BULLET_LIBS "BulletDynamics" "BulletCollision" "BulletSoftBody" "BulletMath")		# Tested with Bullet 2.73
ELSE()
	#SET(BULLET_LIBS "BulletDynamics" "BulletCollision" "BulletSoftBody" "LinearMath" "BulletSingleThreaded")
	SET(BULLET_LIBS "BulletDynamics" "BulletCollision" "BulletSoftBody" "BulletMath" "BulletMultiThreaded")		# Tested with Bullet 2.73
ENDIF()

SET(BULLET_LIBRARY_ERROR "NO")
SET(BULLET_LIBRARY_DEBUG_ERROR "NO")

FOREACH(CUR_LIB ${BULLET_LIBS})
	STRING(TOLOWER "${CUR_LIB}" CUR_LIB_LOWER)
	FIND_LIBRARY(BULLET_LIBRARY_${CUR_LIB}
		NAMES "Lib${CUR_LIB}" "lib${CUR_LIB_LOWER}" ${CUR_LIB} ${CUR_LIB_LOWER}
		HINTS
			$ENV{BULLET_DIR}
			$ENV{BULLET_PATH}
			${ADDITIONAL_SEARCH_PATHS}
		PATH_SUFFIXES lib64 lib src "src/${CUR_LIB}" "src/${CUR_LIB_LOWER}" "out/release_dll${BULLET_MSVC_LIB_DIR}/libs" "out/release${BULLET_MSVC_LIB_DIR}/libs"
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

	FIND_LIBRARY(BULLET_LIBRARY_${CUR_LIB}_DEBUG
		NAMES "Lib${CUR_LIB}d" "lib${CUR_LIB_LOWER}d" "${CUR_LIB}d" "${CUR_LIB_LOWER}d" "Lib${CUR_LIB}_d" "lib${CUR_LIB_LOWER}_d" "${CUR_LIB}_d" "${CUR_LIB_LOWER}_d"
		HINTS
			$ENV{BULLET_DIR}
			$ENV{BULLET_PATH}
			${ADDITIONAL_SEARCH_PATHS}
		PATH_SUFFIXES lib64 lib src "src/${CUR_LIB}" "src/${CUR_LIB_LOWER}" "out/debug_dll${BULLET_MSVC_LIB_DIR}/libs" "out/debug${BULLET_MSVC_LIB_DIR}/libs"
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
	IF(BULLET_LIBRARY_${CUR_LIB} AND NOT BULLET_LIBRARY_ERROR)
		SET(BULLET_LIBRARY "${BULLET_LIBRARY};${BULLET_LIBRARY_${CUR_LIB}}")
	ELSE()
		SET(BULLET_LIBRARY "BULLET_LIBRARY-NOTFOUND")
		SET(BULLET_LIBRARY_ERROR "YES")
	ENDIF()

	IF(BULLET_LIBRARY_${CUR_LIB}_DEBUG AND NOT BULLET_LIBRARY_DEBUG_ERROR)
		SET(BULLET_LIBRARY_DEBUG "${BULLET_LIBRARY_DEBUG};${BULLET_LIBRARY_${CUR_LIB}_DEBUG}")
	ELSE()
		SET(BULLET_LIBRARY_DEBUG "BULLET_LIBRARY_DEBUG-NOTFOUND")
		SET(BULLET_LIBRARY_DEBUG_ERROR "YES")
	ENDIF()
ENDFOREACH()



SET(BULLET_FOUND "NO")
IF(BULLET_LIBRARY AND BULLET_INCLUDE_DIR)
  SET(BULLET_FOUND "YES")
ENDIF()

