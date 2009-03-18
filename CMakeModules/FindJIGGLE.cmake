# Locate Jiggle
# This module defines
# JIGGLE_LIBRARY, JIGGLE_LIBRARY_DEBUG
#JIGGLE_FOUND, if false, do not try to link to Jiggle
# JIGGLE_INCLUDE_DIR, where to find the headers

FIND_PATH(JIGGLE_INCLUDE_DIR jiglib.hpp
	HINTS
	$ENV{JIGGLE_DIR}
	$ENV{JIGGLE_PATH}
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

FIND_LIBRARY(JIGGLE_LIBRARY
	NAMES jiglib jiglibRelease
	HINTS
	$ENV{JIGGLE_DIR}
	$ENV{JIGGLE_PATH}
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

FIND_LIBRARY(JIGGLE_LIBRARY_DEBUG 
	NAMES jiglibd JigLibDebug
	HINTS
	$ENV{JIGGLE_DIR}
	$ENV{JIGGLE_PATH}
	${ADDITIONAL_SEARCH_PATHS}
	PATH_SUFFIXES lib64 lib lib/debug Library
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

SET(JIGGLE_FOUND "NO")
IF(JIGGLE_LIBRARY AND JIGGLE_INCLUDE_DIR)
  SET(JIGGLE_FOUND "YES")
ENDIF()

