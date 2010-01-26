# Extension of the standard FindSDL.cmake
# Adds a finder for the SDL.dll
INCLUDE("${CMAKE_ROOT}/Modules/FindSDL.cmake")

INCLUDE(FindPackageTargetLibraries)
FIND_PACKAGE_SET_STD_INCLUDE_AND_LIBS(SDL)		# Setup SDL_INCLUDE_DIRS and SDL_LIBRARIES (not present in CMake 2.6.3)


# DLL/so copy
IF(PAL_MODULE_COPY)
	SET(SDL_LIBRARY_MODULE_PATH "")
	IF(SDLMAIN_LIBRARY)
		GET_FILENAME_COMPONENT(SDL_LIBRARY_MODULE_PATH ${SDLMAIN_LIBRARY} PATH)
	ENDIF()

	FIND_PATH(SDL_INCLUDE_DIR SDL.h
	HINTS
	$ENV{SDL_DIR}
	$ENV{SDL_PATH}
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

FIND_LIBRARY(SDLMAIN_LIBRARY
	NAMES SDLmain.lib
	HINTS
	$ENV{SDL_DIR}
	$ENV{SDL_PATH}
	${ADDITIONAL_SEARCH_PATHS}
	PATH_SUFFIXES lib64 lib lib/release
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
	
	FIND_FILE(SDL_LIBRARY_MODULE
		NAMES "SDL${MODULE_EXT}"
		HINTS
		$ENV{SDLDIR}
		$ENV{SDL_DIR}
		$ENV{SDL_PATH}
		${ADDITIONAL_SEARCH_PATHS}
		${SDL_LIBRARY_MODULE_PATH}
		PATH_SUFFIXES lib64 lib
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

	# FIND_FILE(SDL_LIBRARY_MODULE_DEBUG 
		# NAMES "SDLd${MODULE_EXT}" "SDL_d${MODULE_EXT}"
		# HINTS
		# $ENV{SDLDIR}
		# $ENV{SDL_DIR}
		# $ENV{SDL_PATH}
		# ${ADDITIONAL_SEARCH_PATHS}
		# ${SDL_LIBRARY_MODULE_PATH}
		# PATH_SUFFIXES lib64 lib
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
