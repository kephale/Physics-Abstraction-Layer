# Locate Novodex/PhysX
# This module defines XXX_FOUND, XXX_INCLUDE_DIRS and XXX_LIBRARIES standard variables

SET(NOVODEX_HEADERS Cooking Foundation Character Physics PhysXLoader)		# Extensions
SET(NOVODEX_INCLUDE_DIRS)
INCLUDE(FindPackageTargetLibraries)

FOREACH(CUR_DIR ${NOVODEX_HEADERS})
	FIND_PATH(NOVODEX_${CUR_DIR}_INCLUDE_DIR
		NAMES "${CUR_DIR}.h" "Nx${CUR_DIR}.h"
		HINTS
		$ENV{NOVODEX_DIR}
		$ENV{NOVODEX_PATH}
		${ADDITIONAL_SEARCH_PATHS}
		PATH_SUFFIXES include Include "SDKs/${CUR_DIR}/include" "SDKs/${CUR_DIR}/Include" "SDKs/Nx${CUR_DIR}/include" "SDKs/Nx${CUR_DIR}/Include"
		PATHS
			~/Library/Frameworks
			/Library/Frameworks
                        /usr/include/PhysX/v2.8.1
                        /usr/include/PhysX/v2.8.0
                        /usr/include/PhysX/v2.7.3
			/usr
			/sw # Fink
			/opt/local # DarwinPorts
			/opt/csw # Blastwave
			/opt
	)

	# Combine all dirs to one variable
	IF(NOVODEX_${CUR_DIR}_INCLUDE_DIR AND NOT NOVODEX_INCLUDE_DIR_ERROR)
		LIST(APPEND NOVODEX_INCLUDE_DIRS "${NOVODEX_${CUR_DIR}_INCLUDE_DIR}")
	ENDIF()
ENDFOREACH()




SET(NOVODEX_LIBS NxCharacter NxCooking PhysXLoader)		# PhysXCore NxPhysics NxFoundation
SET(NOVODEX_LIBRARIES)

FOREACH(CUR_LIB ${NOVODEX_LIBS})
	STRING(TOLOWER "${CUR_LIB}" CUR_LIB_LOWER)
	FIND_LIBRARY(NOVODEX_LIBRARY_${CUR_LIB}
		NAMES ${CUR_LIB} ${CUR_LIB_LOWER}
		HINTS
			$ENV{NOVODEX_DIR}
			$ENV{NOVODEX_PATH}
			${ADDITIONAL_SEARCH_PATHS}
		PATH_SUFFIXES lib/PhysX/v2.8.1 lib/PhysX/v2.8.0 lib/PhysX/v2.7.3 lib64 lib lib/win32 SDKs/lib/win32 latest
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

	# FIND_LIBRARY(NOVODEX_LIBRARY_${CUR_LIB}_DEBUG
		# NAMES "${CUR_LIB}d" "${CUR_LIB_LOWER}d" "${CUR_LIB}_d" "${CUR_LIB_LOWER}_d"
		# HINTS
			# $ENV{NOVODEX_DIR}
			# $ENV{NOVODEX_PATH}
			# ${ADDITIONAL_SEARCH_PATHS}
		# PATH_SUFFIXES lib64 lib lib/win32 SDKs/lib/win32 latest
		# PATHS
			# ~/Library/Frameworks
			# /Library/Frameworks
			# /usr/local
			# /usr
			# /sw
			# /opt/local
			# /opt/csw
			# /opt
	# )

	# Combine all libs to one variable
	IF(NOVODEX_LIBRARY_${CUR_LIB})
		FIND_PACKAGE_ADD_TARGET_LIBRARIES(NOVODEX "${NOVODEX_LIBRARY_${CUR_LIB}}" "${NOVODEX_LIBRARY_${CUR_LIB}_DEBUG}")
	ENDIF()
ENDFOREACH()

# Special for DLL copy
IF(PAL_MODULE_COPY)
	SET(NOVODEX_LIBRARY_MODULE_ERROR "NO")
	SET(NOVODEX_LIBRARY_MODULE_DEBUG_ERROR "NO")

	FOREACH(CUR_LIB ${NOVODEX_LIBS})
		STRING(TOLOWER "${CUR_LIB}" CUR_LIB_LOWER)
		FIND_FILE(NOVODEX_LIBRARY_${CUR_LIB}_MODULE
			NAMES "${CUR_LIB}${MODULE_EXT}" "${CUR_LIB_LOWER}${MODULE_EXT}"
			HINTS
			$ENV{NOVODEX_DIR}
			$ENV{NOVODEX_PATH}
			${ADDITIONAL_SEARCH_PATHS}
			PATH_SUFFIXES bin bin/win32
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

		# FIND_FILE(NOVODEX_LIBRARY_${CUR_LIB}_MODULE_DEBUG 
			# NAMES "${CUR_LIB}d${MODULE_EXT}" "${CUR_LIB_LOWER}d${MODULE_EXT}" "${CUR_LIB}_d${MODULE_EXT}" "${CUR_LIB_LOWER}_d${MODULE_EXT}"
			# HINTS
			# $ENV{NOVODEX_DIR}
			# $ENV{NOVODEX_PATH}
			# ${ADDITIONAL_SEARCH_PATHS}
			# PATH_SUFFIXES bin bin/win32
			# DOC "Optional path of the debug DLL, to be copied after the build."
			# PATHS
				# ~/Library/Frameworks
				# /Library/Frameworks
				# /usr/local
				# /usr
				# /sw
				# /opt/local
				# /opt/csw
				# /opt
		# )

		# Combine all libs to two variables
		IF(NOVODEX_LIBRARY_${CUR_LIB}_MODULE AND NOT NOVODEX_LIBRARY_MODULE_ERROR)
			LIST(APPEND NOVODEX_LIBRARY_MODULE "${NOVODEX_LIBRARY_${CUR_LIB}_MODULE}")
		ELSE()
			SET(NOVODEX_LIBRARY_MODULE "NOVODEX_LIBRARY-NOTFOUND")
			SET(NOVODEX_LIBRARY_MODULE_ERROR "YES")
		ENDIF()

		IF(NOVODEX_LIBRARY_${CUR_LIB}_MODULE_DEBUG AND NOT NOVODEX_LIBRARY_MODULE_DEBUG_ERROR)
			LIST(APPEND NOVODEX_LIBRARY_MODULE_DEBUG "${NOVODEX_LIBRARY_${CUR_LIB}_MODULE_DEBUG}")
		ELSE()
			SET(NOVODEX_LIBRARY_MODULE_DEBUG "NOVODEX_LIBRARY_MODULE_DEBUG-NOTFOUND")
			SET(NOVODEX_LIBRARY_MODULE_DEBUG_ERROR "YES")
		ENDIF()
	ENDFOREACH()
ENDIF()



# handle the QUIETLY and REQUIRED arguments and set CURL_FOUND to TRUE if 
# all listed variables are TRUE
SET(NOVODEX_INCLUDE_DIR_FULL_LIST)
FOREACH(CUR_DIR ${NOVODEX_HEADERS})
	LIST(APPEND NOVODEX_INCLUDE_DIR_FULL_LIST "NOVODEX_${CUR_DIR}_INCLUDE_DIR")
ENDFOREACH()
SET(NOVODEX_LIBRARY_FULL_LIST)
FOREACH(CUR_LIB ${NOVODEX_LIBS})
	LIST(APPEND NOVODEX_LIBRARY_FULL_LIST "NOVODEX_LIBRARY_${CUR_LIB}")
ENDFOREACH()

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(NOVODEX DEFAULT_MSG ${NOVODEX_LIBRARY_FULL_LIST} ${NOVODEX_INCLUDE_DIR_FULL_LIST})

IF(NOVODEX_FOUND)
	# NOVODEX_LIBRARIES has been set before
	# NOVODEX_INCLUDE_DIRS has been set before
ELSE()
	SET(NOVODEX_LIBRARIES)
	SET(NOVODEX_INCLUDE_DIRS)
ENDIF()
