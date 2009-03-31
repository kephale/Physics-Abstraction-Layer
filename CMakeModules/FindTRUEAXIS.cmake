# Locate True Axis
# This module defines XXX_FOUND, XXX_INCLUDE_DIRS and XXX_LIBRARIES standard variables


SET(TRUEAXIS_LIB_NAME "")
IF (${CMAKE_GENERATOR} STREQUAL "vs2008")
	SET(TRUEAXIS_LIB_NAME "9")
ELSEIF (${CMAKE_GENERATOR} STREQUAL "vs2005")
	SET(TRUEAXIS_LIB_NAME "8")
# ELSEIF (${CMAKE_GENERATOR} STREQUAL "vs2003")
	# SET(TRUEAXIS_LIB_NAME "71")
ELSEIF (${CMAKE_GENERATOR} STREQUAL "vs2002")
	SET(TRUEAXIS_LIB_NAME "7")
ELSEIF (${CMAKE_GENERATOR} STREQUAL "vs6")
	SET(TRUEAXIS_LIB_NAME "6")
ENDIF (${CMAKE_GENERATOR} STREQUAL "vs2008")


FIND_PATH(TRUEAXIS_INCLUDE_DIR Physics/Physics.h
	HINTS
	$ENV{TRUEAXIS_DIR}
	$ENV{TRUEAXIS_PATH}
	${ADDITIONAL_SEARCH_PATHS}
	PATH_SUFFIXES include TA
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

FIND_LIBRARY(TRUEAXIS_LIBRARY
	NAMES TA ta "TA_VC${TRUEAXIS_LIB_NAME}"
	HINTS
	$ENV{TRUEAXIS_DIR}
	$ENV{TRUEAXIS_PATH}
	${ADDITIONAL_SEARCH_PATHS}
	PATH_SUFFIXES lib64 lib lib/release TA/Projects TA/Projects/Release
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

FIND_LIBRARY(TRUEAXIS_LIBRARY_DEBUG 
	NAMES TAd tad TA_d ta_d "TA_VC${TRUEAXIS_LIB_NAME}d" "TA_VC${TRUEAXIS_LIB_NAME}_d"
	HINTS
	$ENV{TRUEAXIS_DIR}
	$ENV{TRUEAXIS_PATH}
	${ADDITIONAL_SEARCH_PATHS}
	PATH_SUFFIXES lib64 lib lib/debug TA/Projects TA/Projects/Debug
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
FIND_PACKAGE_HANDLE_STANDARD_ARGS(TRUEAXIS DEFAULT_MSG TRUEAXIS_LIBRARY TRUEAXIS_INCLUDE_DIR)

FIND_PACKAGE_SET_STD_INCLUDE_AND_LIBS(TRUEAXIS)
