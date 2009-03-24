# Locate OpenDynamicsEngine
# This module defines
# ODE_LIBRARY, ODE_LIBRARY_DEBUG
#ODE_FOUND, if false, do not try to link to OpenDynamicsEngine
# ODE_INCLUDE_DIR, where to find the headers

# use SET(ODE_DOUBLE_PRECISION true) to link against double precision ODE

IF (WIN32)
	IF(ODE_DOUBLE_PRECISION)
		SET(ODE_PRECISION_PREFIX "_double")
   	ELSE()
		SET(ODE_PRECISION_PREFIX "_single")
	ENDIF()
ENDIF()

# Try the user's environment request before anything else.
FIND_PATH(ODE_INCLUDE_DIR ode/ode.h
	HINTS
	$ENV{ODE_DIR}
	$ENV{ODE_PATH}
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

FIND_LIBRARY(ODE_LIBRARY
	NAMES "ode${ODE_PRECISION_PREFIX}"
	HINTS
	$ENV{ODE_DIR}
	$ENV{ODE_PATH}
	${ADDITIONAL_SEARCH_PATHS}
	PATH_SUFFIXES lib64 lib "lib/release${ODE_PRECISION_PREFIX}dll"
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

FIND_LIBRARY(ODE_LIBRARY_DEBUG 
	NAMES "ode${ODE_PRECISION_PREFIX}d"
	HINTS
	$ENV{ODE_DIR}
	$ENV{ODE_PATH}
	${ADDITIONAL_SEARCH_PATHS}
	PATH_SUFFIXES "lib/debug${ODE_PRECISION_PREFIX}dll"
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


# Special for DLL copy
IF(PAL_MODULE_COPY)
	FIND_FILE(ODE_LIBRARY_MODULE
		NAMES "ode${ODE_PRECISION_PREFIX}${MODULE_EXT}"
		HINTS
		$ENV{ODE_DIR}
		$ENV{ODE_PATH}
		${ADDITIONAL_SEARCH_PATHS}
		PATH_SUFFIXES "lib/release${ODE_PRECISION_PREFIX}dll"
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

	FIND_FILE(ODE_LIBRARY_MODULE_DEBUG 
		NAMES "ode${ODE_PRECISION_PREFIX}d${MODULE_EXT}"
		HINTS
		$ENV{ODE_DIR}
		$ENV{ODE_PATH}
		${ADDITIONAL_SEARCH_PATHS}
		PATH_SUFFIXES "lib/debug${ODE_PRECISION_PREFIX}dll"
		DOC "Optional path of the debug DLL, to be copied after the build."
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
ENDIF()


SET(ODE_FOUND "NO")
IF(ODE_LIBRARY AND ODE_INCLUDE_DIR)
  SET(ODE_FOUND "YES")
ENDIF()

