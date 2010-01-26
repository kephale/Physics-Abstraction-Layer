# Locate SPE
# This module defines XXX_FOUND, XXX_INCLUDE_DIRS and XXX_LIBRARIES standard variables

FIND_PATH(SPE_INCLUDE_DIR SPE.h
	HINTS
	$ENV{SPE_DIR}
	$ENV{SPE_PATH}
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

FIND_LIBRARY(SPE_LIBRARY
	NAMES SPE
	HINTS
	$ENV{SPE_DIR}
	$ENV{SPE_PATH}
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

# FIND_LIBRARY(SPE_LIBRARY_DEBUG 
	# NAMES SPEd SPE_d
	# HINTS
	# $ENV{SPE_DIR}
	# $ENV{SPE_PATH}
	# ${ADDITIONAL_SEARCH_PATHS}
	# PATH_SUFFIXES lib64 lib lib/debug Library
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


# DLL/so copy
IF(PAL_MODULE_COPY)
	FIND_FILE(SPE_LIBRARY_MODULE
		NAMES "SPE${MODULE_EXT}"
		HINTS
		$ENV{SPE_DIR}
		$ENV{SPE_PATH}
		${ADDITIONAL_SEARCH_PATHS}
		PATH_SUFFIXES bin
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

	# FIND_FILE(SPE_LIBRARY_MODULE_DEBUG 
		# NAMES "SPEd${MODULE_EXT}" "SPE_d${MODULE_EXT}"
		# HINTS
		# $ENV{SPE_DIR}
		# $ENV{SPE_PATH}
		# ${ADDITIONAL_SEARCH_PATHS}
		# PATH_SUFFIXES bin
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
ENDIF()


# handle the QUIETLY and REQUIRED arguments and set CURL_FOUND to TRUE if 
# all listed variables are TRUE
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(SPE DEFAULT_MSG SPE_LIBRARY SPE_INCLUDE_DIR)
INCLUDE(FindPackageTargetLibraries)
FIND_PACKAGE_SET_STD_INCLUDE_AND_LIBS(SPE)
