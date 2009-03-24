# PAL CMake script
# by Sukender (Benoit Neil), based on OSG CMake script
#
# Please note that it is safer to list explicitly all files (h, cpp...) to cinlude rather than a "everything in the directory"-like command.
# As a consequence, you must change the CMake scripts accordingly each time you add/remove/rename a file from any project.
#
# Automatic DLL/so copy is emperimental and not full-featured

# TODO:
# Complete OpenTissue finder (and re-enable OpenTissue with the next release)
# Test finders (To do: Havok, IBDS, Jiggle, OpenTissue, SPE, TrueAxis, Tokamak)
# Test compilation/linkage with dependencies (To do: Box2D, Havok, IBDS, Jiggle, Newton, Novodex, OpenTissue, SPE, TrueAxis, Tokamak)
# Add documentation subproject (?)
# Complete the DLL/so copy, and chack what to do under Unix/Mac

CMAKE_MINIMUM_REQUIRED(VERSION 2.6.2 FATAL_ERROR)
SET(WARNING_STRING "  /!\\ WARNING: ")		#String to prepend to all warnings (Just a way to get the same thing everywhere!)
#SET(ERROR_STRING "")

project( PAL )
SET(PAL_MAJOR_VERSION 0)
SET(PAL_MINOR_VERSION 4)
SET(PAL_PATCH_VERSION 1)

SET(PAL_VERSION ${PAL_MAJOR_VERSION}.${PAL_MINOR_VERSION}.${PAL_PATCH_VERSION})

# List of engines, from less important to more important (The order is used to determine the default engine)
SET(PAL_ENGINE_LIST BOX2D JIGGLE NEWTON OPENTISSUE SPE TRUEAXIS IBDS BULLET TOKAMAK NOVODEX ODE HAVOK)
# List of engines having a "*_palVehicle.cpp" (Subset of PAL_ENGINE_LIST)
SET(PAL_VEHICLED_ENGINE_LIST BULLET NEWTON)

# We have some custom .cmake scripts not in the official distribution.
SET(CMAKE_MODULE_PATH "${PAL_SOURCE_DIR}/CMakeModules;${CMAKE_MODULE_PATH}")

# How are the dependencies by default in CMake? "ON": By default, it select all engines. "OFF": None by default.
# OFF provides a more logical behavior. Thus ON may be used only locally to slightly speed up the meta-make process.
SET(DEPENDENCIES_DEFAULT_OPTION OFF)

SET(PARENT_PATH "${PROJECT_SOURCE_DIR}/..")
SET(BASE_SEARCH_PATHS ${PARENT_PATH})
IF(WIN32)
	SET(BASE_SEARCH_PATHS ${BASE_SEARCH_PATHS} "$ENV{ProgramFiles}")
ENDIF()
SET(PAL_ADDITIONAL_SEARCH_PATHS ${BASE_SEARCH_PATHS} CACHE PATH "Directories or parent directories used to look for dependencies (Such as ODE, Tokamak...), separated by ';'. Not required but helps the script finding them. For instance, if you have '/mydir/bullet' and '/mydir/ode', you may add '/mydir' (as parent dir), or each dir individually.")

# Adds a list of subdirs to all paths in PAL_ADDITIONAL_SEARCH_PATHS and store the result in ADDITIONAL_SEARCH_PATHS
# Call "SET_ADDITIONAL_SEARCH_PATHS(PATH1 PATH2 PATH3 ...)"
MACRO(SET_ADDITIONAL_SEARCH_PATHS)
	SET(ADDITIONAL_SEARCH_PATHS)
	FOREACH(CUR_PATH ${PAL_ADDITIONAL_SEARCH_PATHS})
		SET(ADDITIONAL_SEARCH_PATHS ${ADDITIONAL_SEARCH_PATHS} "${CUR_PATH}")
		FOREACH(CUR_SUBDIR ${ARGN})	
			SET(ADDITIONAL_SEARCH_PATHS ${ADDITIONAL_SEARCH_PATHS} "${CUR_PATH}/${CUR_SUBDIR}")
		ENDFOREACH()	
	ENDFOREACH()
ENDMACRO()

# Option for DLL/so copy
IF(WIN32)
	SET(MODULE_EXT ".dll")
	OPTION(PAL_MODULE_COPY "(EXPERIMENTAL) Set to ON to make PAL build copy the dependencies' modules (DLL/so) to the output directory." ON)
# ELSEIF(UNIX)
	# SET(MODULE_EXT ".so")
	# OPTION(PAL_MODULE_COPY "(EXPERIMENTAL) Set to ON to make PAL build copy the dependencies' modules (DLL/so) to the output directory." ON)
ELSE()
	SET(MODULE_EXT )
	SET(PAL_MODULE_COPY OFF)
ENDIF()


IF(UNIX AND NOT APPLE)
	# Some Unicies need explicit linkage to the Math library or the build fails.
	FIND_LIBRARY(MATH_LIBRARY m)
	FIND_LIBRARY(DL_LIBRARY dl)
	FIND_PACKAGE(Threads)
ENDIF()

OPTION(PAL_BUILD_BOX2D "Set to ON to build PAL BOX2D." ${DEPENDENCIES_DEFAULT_OPTION})
IF(PAL_BUILD_BOX2D AND PAL_CONFIG_HAS_BEEN_RUN_BEFORE)
	SET_ADDITIONAL_SEARCH_PATHS("Box2D")
	FIND_PACKAGE(BOX2D)
ENDIF()

OPTION(PAL_BUILD_BULLET "Set to ON to build PAL BULLET." ${DEPENDENCIES_DEFAULT_OPTION})
OPTION(BULLET_SINGLE_THREADED "Set to ON to search for BULLET library in single threaded mode (multi-threaded otherwise)." OFF)
IF(PAL_BUILD_BULLET AND PAL_CONFIG_HAS_BEEN_RUN_BEFORE)
	SET_ADDITIONAL_SEARCH_PATHS("bullet" "bullet-2.71" "bullet-2.73")
	FIND_PACKAGE(BULLET)
ENDIF()

OPTION(PAL_BUILD_HAVOK "Set to ON to build PAL HAVOK." ${DEPENDENCIES_DEFAULT_OPTION})
IF(PAL_BUILD_HAVOK AND PAL_CONFIG_HAS_BEEN_RUN_BEFORE)
	SET_ADDITIONAL_SEARCH_PATHS("Havok" "hk" "hk550")
	FIND_PACKAGE(HAVOK)
ENDIF()

OPTION(PAL_BUILD_IBDS "Set to ON to build PAL IBDS." ${DEPENDENCIES_DEFAULT_OPTION})
IF(PAL_BUILD_IBDS AND PAL_CONFIG_HAS_BEEN_RUN_BEFORE)
	SET_ADDITIONAL_SEARCH_PATHS("IBDS")
	FIND_PACKAGE(IBDS)
ENDIF()

OPTION(PAL_BUILD_JIGGLE "Set to ON to build PAL JIGGLE." ${DEPENDENCIES_DEFAULT_OPTION})
IF(PAL_BUILD_JIGGLE AND PAL_CONFIG_HAS_BEEN_RUN_BEFORE)
	SET_ADDITIONAL_SEARCH_PATHS("jiglib" "jiglib-0.84")
	FIND_PACKAGE(JIGGLE)
ENDIF()

OPTION(PAL_BUILD_NEWTON "Set to ON to build PAL NEWTON." ${DEPENDENCIES_DEFAULT_OPTION})
OPTION(NEWTON_DOUBLE_PRECISION "Set to ON to search for NEWTON library in double precision." OFF)
IF(PAL_BUILD_NEWTON AND PAL_CONFIG_HAS_BEEN_RUN_BEFORE)
	SET_ADDITIONAL_SEARCH_PATHS("NewtonSDK/sdk")
	FIND_PACKAGE(NEWTON)
ENDIF()

OPTION(PAL_BUILD_NOVODEX "Set to ON to build PAL NOVODEX." ${DEPENDENCIES_DEFAULT_OPTION})
IF(PAL_BUILD_NOVODEX AND PAL_CONFIG_HAS_BEEN_RUN_BEFORE)
	SET_ADDITIONAL_SEARCH_PATHS("PhysX" "NVIDIA Corporation/NVIDIA PhysX SDK/v2.8.1")
	FIND_PACKAGE(NOVODEX)
	# IF(UNIX)
		# FIND_LIBRARY(PTHREAD)
	# ENDIF()
ENDIF()

OPTION(PAL_BUILD_ODE "Set to ON to build PAL ODE." ${DEPENDENCIES_DEFAULT_OPTION})
OPTION(ODE_DOUBLE_PRECISION "Set to ON to search for ODE library in double precision." OFF)
IF(PAL_BUILD_ODE AND PAL_CONFIG_HAS_BEEN_RUN_BEFORE)
	SET_ADDITIONAL_SEARCH_PATHS("ODE" "ode-0.10.0" "ode-0.11")
	FIND_PACKAGE(ODE)
ENDIF()

# OpenTissue DISABLED until the new repository layout is stable
# OPTION(PAL_BUILD_OPENTISSUE "Set to ON to build PAL OPENTISSUE." ${DEPENDENCIES_DEFAULT_OPTION})
# IF(PAL_BUILD_OPENTISSUE AND PAL_CONFIG_HAS_BEEN_RUN_BEFORE)
	# SET_ADDITIONAL_SEARCH_PATHS("opentissue")
	# FIND_PACKAGE(OPENTISSUE)
# ENDIF()

OPTION(PAL_BUILD_SPE "Set to ON to build PAL SPE." ${DEPENDENCIES_DEFAULT_OPTION})
IF(PAL_BUILD_SPE AND PAL_CONFIG_HAS_BEEN_RUN_BEFORE)
	SET_ADDITIONAL_SEARCH_PATHS("SPE_SDK" "SPE_SDK_3.20")
	FIND_PACKAGE(SPE)
ENDIF()

OPTION(PAL_BUILD_TRUEAXIS "Set to ON to build PAL TRUEAXIS." ${DEPENDENCIES_DEFAULT_OPTION})
IF(PAL_BUILD_TRUEAXIS AND PAL_CONFIG_HAS_BEEN_RUN_BEFORE)
	SET_ADDITIONAL_SEARCH_PATHS("TrueAxisPhysicsSDKNonCommercial" "TrueAxisPhysicsSDKNonCommercial1.2.0.3")
	FIND_PACKAGE(TRUEAXIS)
ENDIF()

OPTION(PAL_BUILD_TOKAMAK "Set to ON to build PAL TOKAMAK." ${DEPENDENCIES_DEFAULT_OPTION})
IF(PAL_BUILD_TOKAMAK AND PAL_CONFIG_HAS_BEEN_RUN_BEFORE)
	SET_ADDITIONAL_SEARCH_PATHS("tokamak" "tokamak_release")
	FIND_PACKAGE(TOKAMAK)
ENDIF()

#OPTION(BUILD_EXAMPLES "Set to ON to build PAL examples." ON)

OPTION(PAL_EXAMPLES_DISPLAY "Set to ON to build PAL examples and tests that display something, using dependencies (like OpenGL, SDL...)." ON)
IF(PAL_EXAMPLES_DISPLAY AND PAL_CONFIG_HAS_BEEN_RUN_BEFORE)
	FIND_PACKAGE(OpenGL)
	SET_ADDITIONAL_SEARCH_PATHS("SDL")
	FIND_PACKAGE(SDL)
ENDIF()

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
ENDIF()

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
ELSE()
	SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${OUTPUT_LIBDIR})
ENDIF()

#MARK_AS_ADVANCED(...)

################################################################################

#ONLY windows uses debug suffixes
IF (WIN32)
   SET(CMAKE_DEBUG_POSTFIX "d" CACHE STRING "add a postfix, usually d on windows")
ENDIF()

#ADD_DEFINITIONS(-DOSG_DEBUG_POSTFIX=${CMAKE_DEBUG_POSTFIX})

IF(MSVC)
	ADD_DEFINITIONS("-D_CRT_SECURE_NO_DEPRECATE")
ENDIF()

OPTION(PAL_INTERNAL_DEBUG "Set to ON to enable internal PAL debug (disabled in release configurations)." OFF)
MARK_AS_ADVANCED(PAL_INTERNAL_DEBUG)
# IF(PAL_INTERNAL_DEBUG)
	# ADD_DEFINITIONS("-DINTERNAL_DEBUG")	# Not called since it would be for release configurations too
# ENDIF()


# Adds includes for a library found with FIND_PACKAGE
MACRO(INCLUDE_WITH_VARIABLES TRGTNAME)
	FOREACH(varname ${ARGN})
		IF(${varname}_INCLUDE_DIR)
			INCLUDE_DIRECTORIES("${${varname}_INCLUDE_DIR}")
		ELSEIF(${varname} MATCHES "Boost_.+")		# For Boost
			IF(EXISTS ${Boost_INCLUDE_DIR})		# It doesn't work with IF(${Boost_INCLUDE_DIR}) in CMake 2.6.2!!! Very strange as the variable is not empty
				INCLUDE_DIRECTORIES("${Boost_INCLUDE_DIR}")
			# ELSE()
				# MESSAGE(SEND_ERROR "Missing 'Boost_INCLUDE_DIR' variable")
			ENDIF()
		# ELSE()
			# MESSAGE(SEND_ERROR "Missing '${varname}_INCLUDE_DIR' variable")
		ENDIF()
	ENDFOREACH()
ENDMACRO()

# Adds includes and link properties for a library found with FIND_PACKAGE
MACRO(LINK_WITH_VARIABLES TRGTNAME)
	INCLUDE_WITH_VARIABLES(${ARGV})
	FOREACH(varname ${ARGN})
		IF(${varname}_LIBRARY MATCHES "optimized;.*;debug;.*")
			# Complete rule defined (Mainly for Boost)
			# Actually Boost libraries support auto-linking for some compilers (Including MSVC). However, adding twice the dependency is not an issue.
			TARGET_LINK_LIBRARIES(${TRGTNAME} ${${varname}_LIBRARY})
		ELSE()
			# Otherwise...
			IF(${varname}_LIBRARY_DEBUG)
				#TARGET_LINK_LIBRARIES(${TRGTNAME} optimized "${${varname}_LIBRARY}" debug "${${varname}_LIBRARY_DEBUG}")
				FOREACH(CUR_LIB ${${varname}_LIBRARY})
					TARGET_LINK_LIBRARIES(${TRGTNAME} optimized ${CUR_LIB})
				ENDFOREACH()
				FOREACH(CUR_LIB ${${varname}_LIBRARY_DEBUG})
					TARGET_LINK_LIBRARIES(${TRGTNAME} debug ${CUR_LIB})
				ENDFOREACH()
			ELSE()
				TARGET_LINK_LIBRARIES(${TRGTNAME} ${${varname}_LIBRARY} )
			ENDIF()
		ENDIF()
	ENDFOREACH()
ENDMACRO()

# Sets the given variable to be the default engine, that is to say the name of the higest priority availaible engine.
MACRO(COMPUTE_DEFAULT_ENGINE PAL_DEFAULT_ENGINE)
	SET(PAL_DEFAULT_ENGINE)
	FOREACH(ENGINE ${PAL_ENGINE_LIST})
		IF(PAL_BUILD_${ENGINE})
			SET(PAL_DEFAULT_ENGINE "${ENGINE}")
		ENDIF()
	ENDFOREACH()
ENDMACRO()

# Calls LINK_WITH_VARIABLES for any available engine (used for PAL_STATIC).
MACRO(INCLUDE_WITH_VARIABLES_ALL_ENGINES TARGET_NAME)
	FOREACH(CUR_ENGINE ${PAL_ENGINE_LIST})
		IF(PAL_BUILD_${CUR_ENGINE} AND ${CUR_ENGINE}_FOUND)
			INCLUDE_WITH_VARIABLES( ${TARGET_NAME} ${CUR_ENGINE} )		# We directly link to the engine
		ENDIF()
	ENDFOREACH()
ENDMACRO()

# Calls LINK_WITH_VARIABLES for any available engine (used for PAL_STATIC).
MACRO(LINK_WITH_VARIABLES_ALL_ENGINES TARGET_NAME)
	FOREACH(CUR_ENGINE ${PAL_ENGINE_LIST})
		IF(PAL_BUILD_${CUR_ENGINE} AND ${CUR_ENGINE}_FOUND)
			LINK_WITH_VARIABLES( ${TARGET_NAME} ${CUR_ENGINE} )		# We directly link to the engine
		ENDIF()
	ENDFOREACH()
ENDMACRO()

# Adds INTERNAL_DEBUG preprocessor definition if PAL_INTERNAL_DEBUG is set, in debug configurations only.
MACRO(ADD_INTERNAL_DEBUG_DEFINITIONS TARGET_NAME)
	IF(PAL_INTERNAL_DEBUG)
		GET_TARGET_PROPERTY(CUR_DEFS ${TARGET_NAME} COMPILE_DEFINITIONS)
		IF(NOT CUR_DEFS)
			SET(CUR_DEFS)	# Clear the "NOTFOUND" if this is the case
		ENDIF()
		SET(CUR_DEFS ${CUR_DEFS} INTERNAL_DEBUG)
		SET_TARGET_PROPERTIES(${TARGET_NAME} PROPERTIES COMPILE_DEFINITIONS_DEBUG "${CUR_DEFS}")
	ENDIF()
ENDMACRO()


# Adds a "copy DLL" post-build rule
# This is experimental!
# TODO Under Unices, we should look at *_LIBRARY and *_LIBRARY_DEBUG because we link against the ".so". And we may do a hard link if possible.
FUNCTION(MODULE_FILE_COPY TARGET_NAME DEPENDENCY)
	#SET(FULL_TARGET_NAME "libpal_${TARGET_NAME}")
	SET(FULL_TARGET_NAME "${TARGET_NAME}")

	# Get the output directory
	#GET_TARGET_PROPERTY(TARGET_TYPE ${FULL_TARGET_NAME} TYPE)		# EXECUTABLE, STATIC_LIBRARY, SHARED_LIBRARY, MODULE_LIBRARY, UTILITY, INSTALL_FILES or INSTALL_PROGRAMS
	# IF(TARGET_TYPE STREQUAL "EXECUTABLE")
		# GET_TARGET_PROPERTY(DEST ${FULL_TARGET_NAME} RUNTIME_OUTPUT_DIRECTORY)
	# ELSEIF(TARGET_TYPE STREQUAL "STATIC_LIBRARY")
		# GET_TARGET_PROPERTY(DEST ${FULL_TARGET_NAME} ARCHIVE_OUTPUT_DIRECTORY)
	# ELSEIF(TARGET_TYPE STREQUAL "SHARED_LIBRARY")
		# IF(WIN32)
			# GET_TARGET_PROPERTY(DEST ${FULL_TARGET_NAME} RUNTIME_OUTPUT_DIRECTORY)		# For DLL platforms the DLL part of a shared library is treated as a runtime target and the corresponding import library is treated as an archive target
		# ELSE()
			# GET_TARGET_PROPERTY(DEST ${FULL_TARGET_NAME} LIBRARY_OUTPUT_DIRECTORY)		# For non-DLL platforms shared libraries are treated as library targets
		# ENDIF()
	# ELSEIF(TARGET_TYPE STREQUAL "MODULE_LIBRARY")
		# GET_TARGET_PROPERTY(DEST ${FULL_TARGET_NAME} LIBRARY_OUTPUT_DIRECTORY)
	# ELSE()
		# MESSAGE(SEND_ERROR "MODULE_FILE_COPY is not designed to handle targets others than EXECUTABLE, STATIC_LIBRARY, SHARED_LIBRARY, and MODULE_LIBRARY")
	# ENDIF()

	GET_TARGET_PROPERTY(DEST_RELEASE ${FULL_TARGET_NAME} LOCATION_RELEASE)	# And what about other configurations...?
	STRING(REGEX REPLACE "/[^/]*$" "" DEST_RELEASE "${DEST_RELEASE}")
	#MESSAGE(STATUS "LOCATION_RELEASE: ${DEST_RELEASE}")
	MAKE_DIRECTORY(${DEST_RELEASE})

	GET_TARGET_PROPERTY(DEST_DEBUG ${FULL_TARGET_NAME} LOCATION_DEBUG)
	STRING(REGEX REPLACE "/[^/]*$" "" DEST_DEBUG "${DEST_DEBUG}")
	#MESSAGE(STATUS "LOCATION_DEBUG: ${DEST_DEBUG}")
	MAKE_DIRECTORY(${DEST_DEBUG})

	#IF(NOT CMAKE_BUILD_TYPE STREQUAL "Debug")		# This test doesn't work (CMAKE_BUILD_TYPE isn't intended to be used kike this and may be empty). This is not mandatory, so simply commenting it out is okay.
		# Release
		IF(${DEPENDENCY}_LIBRARY)
			IF(${DEPENDENCY}_LIBRARY_MODULE)
				FOREACH(CUR_MODULE ${${DEPENDENCY}_LIBRARY_MODULE})
					#MESSAGE(STATUS "MODULE_FILE_COPY adds ${CUR_MODULE}")
					ADD_CUSTOM_COMMAND(
						TARGET ${FULL_TARGET_NAME}
						POST_BUILD
						COMMAND ${CMAKE_COMMAND} -E copy ${CUR_MODULE} ${DEST_RELEASE}
					)
				ENDFOREACH()
			ELSE()
				MESSAGE(STATUS "${WARNING_STRING}${DEPENDENCY}_LIBRARY_MODULE not defined. You may have to manually copy ${DEPENDENCY} release DLL to runtime output directory (${DEST_RELEASE}).")
			ENDIF()
		ENDIF()
	#ELSE()
		# Debug
		IF(${DEPENDENCY}_LIBRARY_DEBUG)
			# Add the rule
			IF(${DEPENDENCY}_LIBRARY_MODULE_DEBUG)
				FOREACH(CUR_MODULE ${${DEPENDENCY}_LIBRARY_MODULE_DEBUG})
					#MESSAGE(STATUS "MODULE_FILE_COPY adds ${CUR_MODULE}")
					ADD_CUSTOM_COMMAND(
						TARGET ${FULL_TARGET_NAME}
						POST_BUILD
						COMMAND ${CMAKE_COMMAND} -E copy ${CUR_MODULE} ${DEST_DEBUG}
					)
				ENDFOREACH()
			ELSE()
				#MESSAGE(STATUS "${WARNING_STRING}${DEPENDENCY}_LIBRARY_MODULE_DEBUG not defined. You may have to manually copy ${DEPENDENCY} debug DLL to runtime output directory (${DEST_DEBUG}).")
			ENDIF()

		ELSE()

			IF(${DEPENDENCY}_LIBRARY)
				IF(${DEPENDENCY}_LIBRARY_MODULE)
					FOREACH(CUR_MODULE ${${DEPENDENCY}_LIBRARY_MODULE})
						#MESSAGE(STATUS "MODULE_FILE_COPY adds ${CUR_MODULE}")
						ADD_CUSTOM_COMMAND(
							TARGET ${FULL_TARGET_NAME}
							POST_BUILD
							COMMAND ${CMAKE_COMMAND} -E copy ${CUR_MODULE} ${DEST_DEBUG}
						)
					ENDFOREACH()
				ENDIF()
			ENDIF()
		ENDIF()
	#ENDIF()
ENDFUNCTION(MODULE_FILE_COPY TARGET_NAME DEPENDENCY)

MACRO(IN_STR_LIST ITEM_SEARCHED STR_LIST RETVAL)
	SET(${RETVAL} FALSE)
	FOREACH(CUR_ITEM ${STR_LIST})
		IF(${CUR_ITEM} STREQUAL ${ITEM_SEARCHED})
			SET(${RETVAL} TRUE)
		ENDIF()
	ENDFOREACH()
ENDMACRO()

################################################################################
# Set Config file

COMPUTE_DEFAULT_ENGINE(PAL_DEFAULT_ENGINE)

IF(PAL_STATIC)
	# Sets PAL_ENGINE_INCLUDES to contain preprocessor code for including engines (used for PAL_STATIC).
	# This code may be replaced by something that gets the list of .cpp files elsewhere.
	SET(PAL_ENGINE_INCLUDES)
	FOREACH(CUR_ENGINE ${PAL_ENGINE_LIST})
		IF(PAL_BUILD_${CUR_ENGINE} AND ${CUR_ENGINE}_FOUND)
			STRING(TOLOWER "${CUR_ENGINE}" CUR_ENGINE_LOWER)
			IF(PAL_ENGINE_INCLUDES)
				SET(PAL_ENGINE_INCLUDES "${PAL_ENGINE_INCLUDES}\n")
			ENDIF()
			SET(PAL_ENGINE_INCLUDES "${PAL_ENGINE_INCLUDES}\t#include \"pal_i/${CUR_ENGINE_LOWER}_pal.cpp\"")

			IN_STR_LIST("${CUR_ENGINE}" "${PAL_VEHICLED_ENGINE_LIST}" VEHICLED)
			IF(VEHICLED)
				IF(PAL_ENGINE_INCLUDES)
					SET(PAL_ENGINE_INCLUDES "${PAL_ENGINE_INCLUDES}\n")
				ENDIF()
				SET(PAL_ENGINE_INCLUDES "${PAL_ENGINE_INCLUDES}\t#include \"pal_i/${CUR_ENGINE_LOWER}_palVehicle.cpp\"")
			ENDIF()
		ENDIF()
	ENDFOREACH()

	# Special cases
	# Put here special .cpp to include
ENDIF()

SET(PAL_ENGINE_DEFINES)
FOREACH(CUR_ENGINE ${PAL_ENGINE_LIST})
	IF(PAL_BUILD_${CUR_ENGINE} AND ${CUR_ENGINE}_FOUND)
		STRING(TOLOWER "${CUR_ENGINE}" CUR_ENGINE_LOWER)
		IF(PAL_ENGINE_DEFINES)
			SET(PAL_ENGINE_DEFINES "${PAL_ENGINE_DEFINES}\n")
		ENDIF()
		SET(PAL_ENGINE_DEFINES "${PAL_ENGINE_DEFINES}/// Indicates the CMake build system has found ${CUR_ENGINE}.\n#define PAL_BUILD_${CUR_ENGINE}")
	ENDIF()
ENDFOREACH()

SET(PAL_CONFIG_HEADER "${PROJECT_BINARY_DIR}/pal/Config.h")
CONFIGURE_FILE("${CMAKE_CURRENT_SOURCE_DIR}/pal/Config.h.in" "${PAL_CONFIG_HEADER}")
SET(PAL_CONFIG_STATIC_HEADER "${PROJECT_BINARY_DIR}/pal/ConfigStatic.h")
CONFIGURE_FILE("${CMAKE_CURRENT_SOURCE_DIR}/pal/ConfigStatic.h.in" "${PAL_CONFIG_STATIC_HEADER}")


################################################################################
# Sub-projects

ADD_SUBDIRECTORY(pal)		# libpal
ADD_SUBDIRECTORY(pal_i)		#libpal_ode and such
IF(PAL_EXAMPLES_DISPLAY)
	ADD_SUBDIRECTORY(test_lib)	# libtest
	ADD_SUBDIRECTORY(paldemo)	# paldemo
ENDIF()
IF(PAL_EXAMPLES_NO_DISPLAY)
	ADD_SUBDIRECTORY(example)	# palbeginner
ENDIF()
ADD_SUBDIRECTORY(extras)	# collada and scythe examples (Not in a IF statement since it is done in extra/CMakeLists.txt)



# This needs to be run very last so other parts of the scripts can take advantage of this.
IF(NOT PAL_CONFIG_HAS_BEEN_RUN_BEFORE)
	SET(PAL_CONFIG_HAS_BEEN_RUN_BEFORE 1 CACHE INTERNAL "Flag to track whether this is the first time running CMake or if CMake has been configured before")
ENDIF()
