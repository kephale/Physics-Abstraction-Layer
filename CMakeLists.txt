# PAL CMake script
# by Sukender (Benoit Neil), based on OSG CMake script
#
# Please note that it is safer to list explicitly all files (h, cpp...) to cinlude rather than a "everything in the directory"-like command.
# As a consequence, you must change the CMake scripts accordingly each time you add/remove/rename a file from any project.

# TODO:
# Debug all libs and executables builds with PAL_STATIC
# Complete OpenTissue finder (and re-enable OpenTissue with the next release)
# Test finders (To do: Box2D, Bullet, Havok, IBDS, Jiggle, Newton, Novodex, OpenTissue, SPE, TrueAxis, Tokamak)
# Test compilation/linkage with dependencies (To do: Box2D, Bullet, Havok, IBDS, Jiggle, Newton, Novodex, OpenTissue, SPE, TrueAxis, Tokamak)
# Check on linux & Mac
# Add documentation subproject (?)
# Remove files: begin.cpp, begin_dynamic.cpp, begin_tokamak.cpp, configure.exe, premake.*

CMAKE_MINIMUM_REQUIRED(VERSION 2.6.2 FATAL_ERROR)
SET(WARNING_STRING "  /!\\ WARNING: ")		#String to prepend to all warnings (Just a way to get the same thing everywhere!)
#SET(ERROR_STRING "")

project( PAL )
SET(PAL_MAJOR_VERSION 0)
SET(PAL_MINOR_VERSION 4)
SET(PAL_PATCH_VERSION 0)

SET(PAL_VERSION ${PAL_MAJOR_VERSION}.${PAL_MINOR_VERSION}.${PAL_PATCH_VERSION})

# List of engines, from less important to more important (The order is used to determine the default engine)
SET(PAL_ENGINE_LIST BOX2D JIGGLE NEWTON OPENTISSUE SPE TRUEAXIS IBDS BULLET TOKAMAK NOVODEX ODE HAVOK)

# We have some custom .cmake scripts not in the official distribution.
SET(CMAKE_MODULE_PATH "${PAL_SOURCE_DIR}/CMakeModules;${CMAKE_MODULE_PATH}")

# How are the dependencies by default in CMake? "ON": By default, it select all engines. "OFF": None by default.
# OFF provides a more logical behavior. Thus ON may be used only locally to slightly speed up the meta-make process.
SET(DEPENDENCIES_DEFAULT_OPTION OFF)

SET(PARENT_PATH "${PROJECT_SOURCE_DIR}/..")
SET(BASE_SEARCH_PATHS ${PARENT_PATH})
IF(WIN32)
	SET(BASE_SEARCH_PATHS ${BASE_SEARCH_PATHS} "$ENV{ProgramFiles}")
ENDIF(WIN32)
SET(PAL_ADDITIONAL_SEARCH_PATHS ${BASE_SEARCH_PATHS} CACHE PATH "Base directories to look for dependencies (Such as ODE, Tokamak...), separated by ';'. Not required but helps the script finding them.")

# Adds a list of subdirs to all paths in PAL_ADDITIONAL_SEARCH_PATHS and store the result in ADDITIONAL_SEARCH_PATHS
# Call "SET_ADDITIONAL_SEARCH_PATHS(PATH1 PATH2 PATH3 ...)"
MACRO(SET_ADDITIONAL_SEARCH_PATHS)
	SET(ADDITIONAL_SEARCH_PATHS)
	FOREACH(CUR_PATH ${PAL_ADDITIONAL_SEARCH_PATHS})
		FOREACH(CUR_SUBDIR ${ARGN})	
			SET(ADDITIONAL_SEARCH_PATHS ${ADDITIONAL_SEARCH_PATHS} "${CUR_PATH}/${CUR_SUBDIR}")
		ENDFOREACH(CUR_SUBDIR ${ARGN})	
	ENDFOREACH(CUR_PATH ${PAL_ADDITIONAL_SEARCH_PATHS})
ENDMACRO(SET_ADDITIONAL_SEARCH_PATHS SUBDIR)


IF(UNIX)
	# Some Unicies need explicit linkage to the Math library or the build fails.
	FIND_LIBRARY(MATH_LIBRARY m)
ENDIF(UNIX)

OPTION(PAL_BUILD_BOX2D "Set to ON to build PAL BOX2D." ${DEPENDENCIES_DEFAULT_OPTION})
IF(PAL_BUILD_BOX2D AND PAL_CONFIG_HAS_BEEN_RUN_BEFORE)
	SET_ADDITIONAL_SEARCH_PATHS("Box2D")
	FIND_PACKAGE(BOX2D)
ENDIF(PAL_BUILD_BOX2D AND PAL_CONFIG_HAS_BEEN_RUN_BEFORE)

OPTION(PAL_BUILD_BULLET "Set to ON to build PAL BULLET." ${DEPENDENCIES_DEFAULT_OPTION})
OPTION(BULLET_SINGLE_THREADED "Set to ON to search for BULLET library in single threaded mode (multi-threaded otherwise)." OFF)
IF(PAL_BUILD_BULLET AND PAL_CONFIG_HAS_BEEN_RUN_BEFORE)
	SET_ADDITIONAL_SEARCH_PATHS("bullet" "bullet-2.73")
	FIND_PACKAGE(BULLET)
ENDIF(PAL_BUILD_BULLET AND PAL_CONFIG_HAS_BEEN_RUN_BEFORE)

OPTION(PAL_BUILD_HAVOK "Set to ON to build PAL HAVOK." ${DEPENDENCIES_DEFAULT_OPTION})
IF(PAL_BUILD_HAVOK AND PAL_CONFIG_HAS_BEEN_RUN_BEFORE)
	SET_ADDITIONAL_SEARCH_PATHS("Havok")
	FIND_PACKAGE(HAVOK)
ENDIF(PAL_BUILD_HAVOK AND PAL_CONFIG_HAS_BEEN_RUN_BEFORE)

OPTION(PAL_BUILD_IBDS "Set to ON to build PAL IBDS." ${DEPENDENCIES_DEFAULT_OPTION})
IF(PAL_BUILD_IBDS AND PAL_CONFIG_HAS_BEEN_RUN_BEFORE)
	SET_ADDITIONAL_SEARCH_PATHS("IBDS")
	FIND_PACKAGE(IBDS)
ENDIF(PAL_BUILD_IBDS AND PAL_CONFIG_HAS_BEEN_RUN_BEFORE)

OPTION(PAL_BUILD_JIGGLE "Set to ON to build PAL JIGGLE." ${DEPENDENCIES_DEFAULT_OPTION})
IF(PAL_BUILD_JIGGLE AND PAL_CONFIG_HAS_BEEN_RUN_BEFORE)
	SET_ADDITIONAL_SEARCH_PATHS("jiglib")
	FIND_PACKAGE(JIGGLE)
ENDIF(PAL_BUILD_JIGGLE AND PAL_CONFIG_HAS_BEEN_RUN_BEFORE)

OPTION(PAL_BUILD_NEWTON "Set to ON to build PAL NEWTON." ${DEPENDENCIES_DEFAULT_OPTION})
IF(PAL_BUILD_NEWTON AND PAL_CONFIG_HAS_BEEN_RUN_BEFORE)
	SET_ADDITIONAL_SEARCH_PATHS("NewtonSDK/sdk")
	FIND_PACKAGE(NEWTON)
ENDIF(PAL_BUILD_NEWTON AND PAL_CONFIG_HAS_BEEN_RUN_BEFORE)

OPTION(PAL_BUILD_NOVODEX "Set to ON to build PAL NOVODEX." ${DEPENDENCIES_DEFAULT_OPTION})
IF(PAL_BUILD_NOVODEX AND PAL_CONFIG_HAS_BEEN_RUN_BEFORE)
	SET_ADDITIONAL_SEARCH_PATHS("PhysX")
	FIND_PACKAGE(NOVODEX)
ENDIF(PAL_BUILD_NOVODEX AND PAL_CONFIG_HAS_BEEN_RUN_BEFORE)

OPTION(PAL_BUILD_ODE "Set to ON to build PAL ODE." ${DEPENDENCIES_DEFAULT_OPTION})
OPTION(ODE_DOUBLE_PRECISION "Set to ON to search for ODE library in double precision." OFF)
IF(PAL_BUILD_ODE AND PAL_CONFIG_HAS_BEEN_RUN_BEFORE)
	SET_ADDITIONAL_SEARCH_PATHS("ODE" "ode-0.10.0" "ode-0.11")
	FIND_PACKAGE(ODE)
ENDIF(PAL_BUILD_ODE AND PAL_CONFIG_HAS_BEEN_RUN_BEFORE)

# OpenTissue DISABLED until the new repository layout is stable
# OPTION(PAL_BUILD_OPENTISSUE "Set to ON to build PAL OPENTISSUE." ${DEPENDENCIES_DEFAULT_OPTION})
# IF(PAL_BUILD_OPENTISSUE AND PAL_CONFIG_HAS_BEEN_RUN_BEFORE)
	# SET_ADDITIONAL_SEARCH_PATHS("opentissue")
	# FIND_PACKAGE(OPENTISSUE)
# ENDIF(PAL_BUILD_OPENTISSUE AND PAL_CONFIG_HAS_BEEN_RUN_BEFORE)

OPTION(PAL_BUILD_SPE "Set to ON to build PAL SPE." ${DEPENDENCIES_DEFAULT_OPTION})
IF(PAL_BUILD_SPE AND PAL_CONFIG_HAS_BEEN_RUN_BEFORE)
	SET_ADDITIONAL_SEARCH_PATHS("SPE_SDK")
	FIND_PACKAGE(SPE)
ENDIF(PAL_BUILD_SPE AND PAL_CONFIG_HAS_BEEN_RUN_BEFORE)

OPTION(PAL_BUILD_TRUEAXIS "Set to ON to build PAL TRUEAXIS." ${DEPENDENCIES_DEFAULT_OPTION})
IF(PAL_BUILD_TRUEAXIS AND PAL_CONFIG_HAS_BEEN_RUN_BEFORE)
	SET_ADDITIONAL_SEARCH_PATHS("TrueAxisPhysicsSDKNonCommercial")
	FIND_PACKAGE(TRUEAXIS)
ENDIF(PAL_BUILD_TRUEAXIS AND PAL_CONFIG_HAS_BEEN_RUN_BEFORE)

OPTION(PAL_BUILD_TOKAMAK "Set to ON to build PAL TOKAMAK." ${DEPENDENCIES_DEFAULT_OPTION})
IF(PAL_BUILD_TOKAMAK AND PAL_CONFIG_HAS_BEEN_RUN_BEFORE)
	SET_ADDITIONAL_SEARCH_PATHS("tokamak")
	FIND_PACKAGE(TOKAMAK)
ENDIF(PAL_BUILD_TOKAMAK AND PAL_CONFIG_HAS_BEEN_RUN_BEFORE)

#OPTION(BUILD_EXAMPLES "Set to ON to build PAL examples." ON)

OPTION(PAL_EXAMPLES_DISPLAY "Set to ON to build PAL examples and tests that display something, using dependencies (like OpenGL, SDL...)." ON)
IF(PAL_EXAMPLES_DISPLAY AND PAL_CONFIG_HAS_BEEN_RUN_BEFORE)
	FIND_PACKAGE(OpenGL)
	FIND_PACKAGE(SDL)
ENDIF(PAL_EXAMPLES_DISPLAY AND PAL_CONFIG_HAS_BEEN_RUN_BEFORE)

OPTION(PAL_EXAMPLES_NO_DISPLAY "Set to ON to build PAL examples and tests that don't have dependencies (like OpenGL, SDL...)." ON)

OPTION(PAL_STATIC "Set to ON to build static libraries (lib) instead of shared ones (DLL/so)." OFF)

OPTION(BUILD_OUTPUT_DIR_RELEASE_DEBUG "Set to OFF to build binaries in the same directory (ex: /bin or /lib), instead of separating them (ex: /bin/debug, /bin/release, /lib/debug, /lib/release)." ON)
MARK_AS_ADVANCED(BUILD_OUTPUT_DIR_RELEASE_DEBUG)


################################################################################
# Directories used for projects

SET(HEADERS_BASE_PATH "${PAL_SOURCE_DIR}/pal")
SET(SOURCE_BASE_PATH "${PAL_SOURCE_DIR}/pal")
SET(HEADERS_FRAMEWORK_PATH "${PAL_SOURCE_DIR}/framework")
SET(SOURCE_FRAMEWORK_PATH "${PAL_SOURCE_DIR}/framework")
SET(HEADERS_IMPL_PATH "${PAL_SOURCE_DIR}/pal_i")
SET(SOURCE_IMPL_PATH "${PAL_SOURCE_DIR}/pal_i")

SET(HEADERS_TEST_LIB_PATH "${PAL_SOURCE_DIR}/test_lib")
SET(SOURCE_TEST_LIB_PATH "${PAL_SOURCE_DIR}/test_lib")
SET(HEADERS_EXAMPLE_PATH "${PAL_SOURCE_DIR}/example")
SET(SOURCE_EXAMPLE_PATH "${PAL_SOURCE_DIR}/example")
SET(HEADERS_SDLGL_PATH "${PAL_SOURCE_DIR}/sdlgl")
SET(SOURCE_SDLGL_PATH "${PAL_SOURCE_DIR}/sdlgl")


# Make the headers visible to everything
IF(NOT ${PROJECT_BINARY_DIR} EQUAL ${PROJECT_SOURCE_DIR})
   INCLUDE_DIRECTORIES(${PROJECT_BINARY_DIR})
ENDIF(NOT ${PROJECT_BINARY_DIR} EQUAL ${PROJECT_SOURCE_DIR})

INCLUDE_DIRECTORIES(
	${PROJECT_SOURCE_DIR}
)

################################################################################
# Create bin and lib directories if required

SET(OUTPUT_BINDIR ${PROJECT_BINARY_DIR}/bin)
MAKE_DIRECTORY(${OUTPUT_BINDIR})
SET(OUTPUT_LIBDIR ${PROJECT_BINARY_DIR}/lib)
MAKE_DIRECTORY(${OUTPUT_LIBDIR})

SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${OUTPUT_LIBDIR})
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${OUTPUT_BINDIR})
IF(WIN32)
	SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${OUTPUT_BINDIR})
ELSE(WIN32)
	SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${OUTPUT_LIBDIR})
ENDIF(WIN32)

#MARK_AS_ADVANCED(...)

################################################################################

SET(CMAKE_DEBUG_POSTFIX "d" CACHE STRING "add a postfix, usually d on windows")
#ADD_DEFINITIONS(-DOSG_DEBUG_POSTFIX=${CMAKE_DEBUG_POSTFIX})

IF(MSVC)
	ADD_DEFINITIONS("-D_CRT_SECURE_NO_DEPRECATE")
ENDIF(MSVC)

OPTION(PAL_INTERNAL_DEBUG "Set to ON to enable internal PAL debug (disabled in release configurations)." OFF)
MARK_AS_ADVANCED(PAL_INTERNAL_DEBUG)
IF(PAL_INTERNAL_DEBUG)
	IF(CMAKE_BUILD_TYPE STREQUAL "Debug")
		ADD_DEFINITIONS("-DINTERNAL_DEBUG")
	ENDIF(CMAKE_BUILD_TYPE STREQUAL "Debug")
ENDIF(PAL_INTERNAL_DEBUG)


IF(UNIX AND NOT WIN32)
	IF(CMAKE_BUILD_TYPE STREQUAL "Debug")
		ADD_DEFINITIONS("-D_DEBUG")
	ELSE(CMAKE_BUILD_TYPE STREQUAL "Debug")
		ADD_DEFINITIONS("-DNDEBUG")
	ENDIF(CMAKE_BUILD_TYPE STREQUAL "Debug")
ENDIF(UNIX AND NOT WIN32)

IF(CYGWIN)
	IF(CMAKE_BUILD_TYPE STREQUAL "Debug")
		ADD_DEFINITIONS("-D_DEBUG")
	ENDIF(CMAKE_BUILD_TYPE STREQUAL "Debug")
ENDIF(CYGWIN)


# Adds includes for a library found with FIND_PACKAGE
MACRO(INCLUDE_WITH_VARIABLES TRGTNAME)
	FOREACH(varname ${ARGN})
		IF(${varname}_INCLUDE_DIR)
			INCLUDE_DIRECTORIES("${${varname}_INCLUDE_DIR}")
		ELSEIF(${varname} MATCHES "Boost_.+")		# For Boost
			IF(EXISTS ${Boost_INCLUDE_DIR})		# It doesn't work with IF(${Boost_INCLUDE_DIR}) in CMake 2.6.2!!! Very strange as the variable is not empty
				INCLUDE_DIRECTORIES("${Boost_INCLUDE_DIR}")
			# ELSE(EXISTS ${Boost_INCLUDE_DIR})
				# MESSAGE(SEND_ERROR "Missing 'Boost_INCLUDE_DIR' variable")
			ENDIF(EXISTS ${Boost_INCLUDE_DIR})
		# ELSE(${varname}_INCLUDE_DIR)
			# MESSAGE(SEND_ERROR "Missing '${varname}_INCLUDE_DIR' variable")
		ENDIF(${varname}_INCLUDE_DIR)
	ENDFOREACH(varname)
ENDMACRO(INCLUDE_WITH_VARIABLES TRGTNAME)

# Adds includes and link properties for a library found with FIND_PACKAGE
MACRO(LINK_WITH_VARIABLES TRGTNAME)
	INCLUDE_WITH_VARIABLES(${ARGV})
	FOREACH(varname ${ARGN})
		IF(${varname}_LIBRARY MATCHES "optimized;.*;debug;.*")
			# Complete rule defined (Mainly for Boost)
			# Actually Boost libraries support auto-linking for some compilers (Including MSVC). However, adding twice the dependency is not an issue.
			TARGET_LINK_LIBRARIES(${TRGTNAME} ${${varname}_LIBRARY})
		ELSE(${varname}_LIBRARY MATCHES "optimized;.*;debug;.*")
			# Otherwise...
			IF(${varname}_LIBRARY_DEBUG)
				#TARGET_LINK_LIBRARIES(${TRGTNAME} optimized "${${varname}_LIBRARY}" debug "${${varname}_LIBRARY_DEBUG}")
				FOREACH(CUR_LIB ${${varname}_LIBRARY})
					TARGET_LINK_LIBRARIES(${TRGTNAME} optimized ${CUR_LIB})
				ENDFOREACH(CUR_LIB ${${varname}_LIBRARY})
				FOREACH(CUR_LIB ${${varname}_LIBRARY_DEBUG})
					TARGET_LINK_LIBRARIES(${TRGTNAME} debug ${CUR_LIB})
				ENDFOREACH(CUR_LIB ${${varname}_LIBRARY_DEBUG})
			ELSE(${varname}_LIBRARY_DEBUG)
				TARGET_LINK_LIBRARIES(${TRGTNAME} ${${varname}_LIBRARY} )
			ENDIF(${varname}_LIBRARY_DEBUG)
		ENDIF(${varname}_LIBRARY MATCHES "optimized;.*;debug;.*")
	ENDFOREACH(varname)
ENDMACRO(LINK_WITH_VARIABLES TRGTNAME)


MACRO(COMPUTE_DEFAULT_ENGINE PAL_DEFAULT_ENGINE)
	FOREACH(ENGINE ${PAL_ENGINE_LIST})
		IF(PAL_BUILD_${ENGINE})
			SET(PAL_DEFAULT_ENGINE "${ENGINE}")
		ENDIF(PAL_BUILD_${ENGINE})
	ENDFOREACH(ENGINE ${PAL_ENGINE_LIST})
ENDMACRO(COMPUTE_DEFAULT_ENGINE PAL_DEFAULT_ENGINE)


# Adds a "copy DLL" post-build rule
FUNCTION(MODULE_FILE_COPY TARGET_NAME DEPENDENCY)
	#SET(FULL_TARGET_NAME "libpal_${TARGET_NAME}")
	SET(FULL_TARGET_NAME "${TARGET_NAME}")
	GET_PROPERTY(DEST
		TARGET ${FULL_TARGET_NAME}
		PROPERTY RUNTIME_OUTPUT_DIRECTORY
	)
	#IF(NOT CMAKE_BUILD_TYPE STREQUAL "Debug")		# This test doesn't work... and I don't know why. This is not mandatory, so simply commenting it out is okay.
		# Release
		IF(${DEPENDENCY}_LIBRARY)
			IF(${DEPENDENCY}_LIBRARY_MODULE)
				ADD_CUSTOM_COMMAND(
					TARGET ${FULL_TARGET_NAME}
					POST_BUILD
					COMMAND ${CMAKE_COMMAND} -E copy ${${DEPENDENCY}_LIBRARY_MODULE} ${DEST}
				)
			ELSE(${DEPENDENCY}_LIBRARY_MODULE)
				MESSAGE(STATUS "${WARNING_STRING}${DEPENDENCY}_LIBRARY_MODULE not defined. You may have to manually copy ${DEPENDENCY} release DLL to runtime output directory (${DEST}).")
			ENDIF(${DEPENDENCY}_LIBRARY_MODULE)
		ENDIF(${DEPENDENCY}_LIBRARY)
	#ELSE(NOT CMAKE_BUILD_TYPE STREQUAL "Debug")
		# Debug
		IF(${DEPENDENCY}_LIBRARY_DEBUG)
			# Test if debug and release are the same file (This should not be!)
			# Test is a simple STREQUAL since paths are both given by FIND_FILE
			IF(${DEPENDENCY}_LIBRARY_MODULE_DEBUG STREQUAL ${DEPENDENCY}_LIBRARY_MODULE)
				# Warn the user
				MESSAGE(STATUS "${WARNING_STRING}${DEPENDENCY}_LIBRARY_MODULE_DEBUG and ${DEPENDENCY}_LIBRARY_MODULE are identical (${${DEPENDENCY}_LIBRARY_MODULE_DEBUG}). Debug DLL will thus not be copied to runtime output directory (${DEST}).")
			ELSE(${DEPENDENCY}_LIBRARY_MODULE_DEBUG STREQUAL ${DEPENDENCY}_LIBRARY_MODULE)
				# Add the rule
				IF(${DEPENDENCY}_LIBRARY_MODULE_DEBUG)
					ADD_CUSTOM_COMMAND(
						TARGET ${FULL_TARGET_NAME}
						POST_BUILD
						COMMAND ${CMAKE_COMMAND} -E copy ${${DEPENDENCY}_LIBRARY_MODULE_DEBUG} ${DEST}
					)
				ELSE(${DEPENDENCY}_LIBRARY_MODULE_DEBUG)
					MESSAGE(STATUS "${WARNING_STRING}${DEPENDENCY}_LIBRARY_MODULE_DEBUG not defined. You may have to manually copy ${DEPENDENCY} debug DLL to runtime output directory (${DEST}).")
				ENDIF(${DEPENDENCY}_LIBRARY_MODULE_DEBUG)
			ENDIF(${DEPENDENCY}_LIBRARY_MODULE_DEBUG STREQUAL ${DEPENDENCY}_LIBRARY_MODULE)
		ENDIF(${DEPENDENCY}_LIBRARY_DEBUG)
	#ENDIF(NOT CMAKE_BUILD_TYPE STREQUAL "Debug")
ENDFUNCTION(MODULE_FILE_COPY TARGET_NAME DEPENDENCY)

################################################################################
# Set Config file

# SET(PAL_CONFIG_HEADER "${PROJECT_BINARY_DIR}/Include/PAL/Config.h")
# CONFIGURE_FILE("${CMAKE_CURRENT_SOURCE_DIR}/Src/Config.in" "${PAL_CONFIG_HEADER}")



################################################################################
# Sub-projects

ADD_SUBDIRECTORY(pal)		# libpal
ADD_SUBDIRECTORY(pal_i)		#libpal_ode and such
IF(PAL_EXAMPLES_DISPLAY)
	ADD_SUBDIRECTORY(test_lib)	# libtest
	ADD_SUBDIRECTORY(paldemo)	# paldemo
ENDIF(PAL_EXAMPLES_DISPLAY)
IF(PAL_EXAMPLES_NO_DISPLAY)
	ADD_SUBDIRECTORY(example)	# palbeginner
ENDIF(PAL_EXAMPLES_NO_DISPLAY)
ADD_SUBDIRECTORY(extras)	# collada and scythe examples (Not in a IF statement since it is done in extra/CMakeLists.txt)



# This needs to be run very last so other parts of the scripts can take advantage of this.
IF(NOT PAL_CONFIG_HAS_BEEN_RUN_BEFORE)
	SET(PAL_CONFIG_HAS_BEEN_RUN_BEFORE 1 CACHE INTERNAL "Flag to track whether this is the first time running CMake or if CMake has been configured before")
ENDIF(NOT PAL_CONFIG_HAS_BEEN_RUN_BEFORE)
