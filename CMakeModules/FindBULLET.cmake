# Locate Bullet
# This module defines XXX_FOUND, XXX_INCLUDE_DIRS and XXX_LIBRARIES standard variables
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
	SET(BULLET_LIBS "BulletDynamics" "BulletCollision" "BulletSoftBody" "BulletMath")		# Tested with Bullet 2.74
ELSE()
	#SET(BULLET_LIBS "BulletDynamics" "BulletCollision" "BulletSoftBody" "LinearMath" "BulletSingleThreaded")
	SET(BULLET_LIBS "BulletDynamics" "BulletCollision" "BulletSoftBody" "BulletMath" "BulletMultiThreaded")		# Tested with Bullet 2.74
ENDIF()
SET(BULLET_LIBRARIES)

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

	# Combine all libs to one variable
	IF(BULLET_LIBRARY_${CUR_LIB})
		FIND_PACKAGE_ADD_TARGET_LIBRARIES(BULLET "${BULLET_LIBRARY_${CUR_LIB}}" "${BULLET_LIBRARY_${CUR_LIB}_DEBUG}")
	ENDIF()
ENDFOREACH()



# handle the QUIETLY and REQUIRED arguments and set CURL_FOUND to TRUE if 
# all listed variables are TRUE
SET(BULLET_LIBRARY_FULL_LIST)
FOREACH(CUR_LIB ${BULLET_LIBS})
	LIST(APPEND BULLET_LIBRARY_FULL_LIST "BULLET_LIBRARY_${CUR_LIB}")
ENDFOREACH()

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(BULLET DEFAULT_MSG ${BULLET_LIBRARY_FULL_LIST} BULLET_INCLUDE_DIR)

IF(BULLET_FOUND)
	# BULLET_LIBRARIES has been set before
	SET(BULLET_INCLUDE_DIRS ${BULLET_INCLUDE_DIR})
ELSE()
	SET(BULLET_LIBRARIES)
	SET(BULLET_INCLUDE_DIRS)
ENDIF()
