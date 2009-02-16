# Locate OpenDynamicsEngine
# This module defines
# ODE_LIBRARY, ODE_LIBRARY_DEBUG
#ODE_FOUND, if false, do not try to link to OpenDynamicsEngine
# ODE_INCLUDE_DIR, where to find the headers

# use SET(ODE_DOUBLE_PRECISION true) to link against double precision ODE

IF(ODE_DOUBLE_PRECISION)
	SET(ODE_PRECISION_PREFIX "double")
ELSE(ODE_DOUBLE_PRECISION)
	SET(ODE_PRECISION_PREFIX "single")
ENDIF(ODE_DOUBLE_PRECISION)


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
	NAMES "ode_${ODE_PRECISION_PREFIX}"
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
	NAMES "ode_${ODE_PRECISION_PREFIX}d"
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
IF(WIN32)
	FIND_FILE(ODE_LIBRARY_MODULE
		NAMES "ode_${ODE_PRECISION_PREFIX}.dll"
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
		NAMES "ode_${ODE_PRECISION_PREFIX}d.dll"
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
ENDIF(WIN32)


SET(ODE_FOUND "NO")
IF(ODE_LIBRARY AND ODE_INCLUDE_DIR)
  SET(ODE_FOUND "YES")
ENDIF(ODE_LIBRARY AND ODE_INCLUDE_DIR)

