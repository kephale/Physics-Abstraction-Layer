# PAL CMake script
# by Sukender (Benoit Neil), based on OSG CMake script
#
# Please note that it is safer to list explicitly all files (h, cpp...) to include rather than a "everything in the directory"-like command.
# As a consequence, you must change the CMake scripts accordingly each time you add/remove/rename a file from any project.
#
# Automatic DLL/so copy is emperimental and not full-featured

# TODO:
# Add some macros in libpal to allow building libpal as a DLL (the __declspec stuff for MSVC...)
# Make PAL work in static with multiple engines
# Complete OpenTissue finder (and re-enable OpenTissue with the next release)
# Test finders (To do: Havok, IBDS, Jiggle, OpenTissue, SPE, TrueAxis, Tokamak)
# Test compilation/linkage with dependencies (To do: Havok, Jiggle, Newton, OpenTissue, SPE, TrueAxis, Tokamak)
# Add documentation subproject (?)
# Complete the DLL/so copy, and chack what to do under Unix/Mac
# Wait for CMake to give the ability to have INCLUDE_DIRECTORIES as a target property (and not a directory property), and change scripts accordingly, to avoid libpal_* to have all engines' include dirs.

project( PAL )
CMAKE_MINIMUM_REQUIRED(VERSION 2.8.0 FATAL_ERROR)

FIND_PACKAGE(GTest)
IF(GTEST_FOUND)
	ENABLE_TESTING()
	INCLUDE(CTest)
	SET(TESTING_ENABLED TRUE)
ELSE()
	MESSAGE(STATUS "Google Test framework not found. Unit tests not enabled.")
ENDIF()
	

SET(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/CMakeModules;${CMAKE_MODULE_PATH}")
include(Utilities)
include(CMakeDependentOption)

SET(WARNING_STRING "  /!\\ WARNING: ")		#String to prepend to all warnings (Just a way to get the same thing everywhere!)
#SET(ERROR_STRING "")

# Read PAL version numbers
SET(PAL_VERSION_FILE_PATH "pal/ConfigVersion.h")
FILE(READ ${PAL_VERSION_FILE_PATH} VERSION_FILE_TEXT)
STRING(REGEX MATCH "#define PAL_SDK_VERSION_MAJOR[ \t]+([0-9]+)"  PAL_MAJOR_VERSION ${VERSION_FILE_TEXT})
STRING(REGEX MATCH "#define PAL_SDK_VERSION_MINOR[ \t]+([0-9]+)"  PAL_MINOR_VERSION ${VERSION_FILE_TEXT})
STRING(REGEX MATCH "#define PAL_SDK_VERSION_BUGFIX[ \t]+([0-9]+)" PAL_PATCH_VERSION ${VERSION_FILE_TEXT})
IF(NOT PAL_MAJOR_VERSION OR NOT PAL_MINOR_VERSION OR NOT PAL_PATCH_VERSION)
	MESSAGE(SEND_ERROR "Could not read version numbers from ${PAL_VERSION_FILE_PATH}")
ENDIF()
STRING(REGEX REPLACE ".+([0-9]+)" "\\1" PAL_MAJOR_VERSION ${PAL_MAJOR_VERSION})
STRING(REGEX REPLACE ".+([0-9]+)" "\\1" PAL_MINOR_VERSION ${PAL_MINOR_VERSION})
STRING(REGEX REPLACE ".+([0-9]+)" "\\1" PAL_PATCH_VERSION ${PAL_PATCH_VERSION})
SET(PAL_VERSION_COUNT 3)

SET(PAL_VERSION ${PAL_MAJOR_VERSION}.${PAL_MINOR_VERSION}.${PAL_PATCH_VERSION})
#MESSAGE(STATUS "PAL v${PAL_VERSION}")


# List of engines, from less important to more important (The order is used to determine the default engine)
SET(PAL_ENGINE_LIST BOX2D JIGGLE NEWTON OPENTISSUE SPE TRUEAXIS IBDS BULLET TOKAMAK NOVODEX ODE HAVOK)
# List of engines having a "*_palVehicle.cpp" (Subset of PAL_ENGINE_LIST)
SET(PAL_VEHICLED_ENGINE_LIST BULLET NEWTON)

# We have some custom .cmake scripts not in the official distribution.

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
OPTION(BULLET_DOUBLE_PRECISION "Set to ON if bullet has been build double precision." OFF)
IF(PAL_BUILD_BULLET AND PAL_CONFIG_HAS_BEEN_RUN_BEFORE)
	SET_ADDITIONAL_SEARCH_PATHS("bullet" "bullet-2.71" "bullet-2.73" "bullet-2.74" "bullet-2.75" "bullet-2.75-rc3")
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
	SET_ADDITIONAL_SEARCH_PATHS("ODE" "ode-0.10.0" "ode-0.11" "ode-0.11.1")
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

CMAKE_DEPENDENT_OPTION(PAL_EXAMPLES_DISPLAY "Set to ON to build PAL examples and tests that display something, using dependencies (like OpenGL, SDL...)." ON "WIN32" OFF)

IF(PAL_EXAMPLES_DISPLAY AND PAL_CONFIG_HAS_BEEN_RUN_BEFORE)
	FIND_PACKAGE(OpenGL)
	SET_ADDITIONAL_SEARCH_PATHS("SDL" "SDL-1.2.13")
	FIND_PACKAGE(SDL)
ENDIF()

OPTION(PAL_EXAMPLES_NO_DISPLAY "Set to ON to build PAL examples and tests that don't have dependencies (like OpenGL, SDL...)." ON)

OPTION(PAL_STATIC "Set to ON to build static libraries (lib) instead of shared ones (DLL/so)." OFF)
IF(PAL_STATIC)
	ADD_DEFINITIONS(-DPAL_STATIC)
ENDIF()

OPTION(BUILD_OUTPUT_DIR_RELEASE_DEBUG "Set to OFF to build binaries in the same directory (ex: /bin or /lib), instead of separating them (ex: /bin/debug, /bin/release, /lib/debug, /lib/release)." ON)
MARK_AS_ADVANCED(BUILD_OUTPUT_DIR_RELEASE_DEBUG)


################################################################################
# Directories used for projects

SET(HEADERS_BASE_DIR_NAME "pal")
SET(HEADERS_FRAMEWORK_DIR_NAME "framework")
SET(HEADERS_IMPL_DIR_NAME "pal_i")

SET(HEADERS_BASE_PATH "${PAL_SOURCE_DIR}/${HEADERS_BASE_DIR_NAME}")
SET(SOURCE_BASE_PATH "${PAL_SOURCE_DIR}/pal")
SET(HEADERS_FRAMEWORK_PATH "${PAL_SOURCE_DIR}/${HEADERS_FRAMEWORK_DIR_NAME}")
SET(SOURCE_FRAMEWORK_PATH "${PAL_SOURCE_DIR}/framework")
SET(HEADERS_IMPL_PATH "${PAL_SOURCE_DIR}/${HEADERS_IMPL_DIR_NAME}")
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
	${PROJECT_BINARY_DIR}
)

################################################################################
# CMake doesn't set this itself :(
#
# LIB_SUFFIX is more standard, but our ModuleInstall.cmake uses LIB_POSTFIX
IF(CMAKE_SYSTEM_NAME STREQUAL "Linux" AND CMAKE_SIZEOF_VOID_P EQUAL 8)
	SET(LIB_SUFFIX 64)
	SET(LIB_POSTFIX 64)
	SET(FIND_LIBRARY_USE_LIB64_PATHS TRUE)
ELSE()
	SET(LIB_SUFFIX "")
	SET(LIB_POSTFIX "")
ENDIF()

################################################################################
# Create bin and lib directories if required
SET(OUTPUT_BINDIR ${PROJECT_BINARY_DIR}/bin)
MAKE_DIRECTORY(${OUTPUT_BINDIR})
SET(OUTPUT_LIBDIR ${PROJECT_BINARY_DIR}/lib${LIB_SUFFIX})
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

IF(APPLE)
	SET(CMAKE_SHARED_LINKER_FLAGS "-Wl,-single_module -flat_namespace -compatibility_version 2 -current_version 2" )
	INCLUDE_DIRECTORIES(/usr/include/malloc)
ENDIF()

OPTION(PAL_INTERNAL_DEBUG "Set to ON to enable internal PAL debug (disabled in release configurations)." OFF)
MARK_AS_ADVANCED(PAL_INTERNAL_DEBUG)
# IF(PAL_INTERNAL_DEBUG)
	# ADD_DEFINITIONS("-DINTERNAL_DEBUG")	# Not called since it would be for release configurations too
# ENDIF()

FUNCTION(ADD_TARGET_PROPERTIES TARGET_NAME PROPERTY_NAME)
	GET_TARGET_PROPERTY(CURRENT_PROPERTY ${TARGET_NAME} ${PROPERTY_NAME})
	IF(CURRENT_PROPERTY)
		SET_TARGET_PROPERTIES(${TARGET_NAME} PROPERTIES ${PROPERTY_NAME} "${CURRENT_PROPERTY};${ARGN}")
	ELSE()
		SET_TARGET_PROPERTIES(${TARGET_NAME} PROPERTIES ${PROPERTY_NAME} "${ARGN}")
	ENDIF()
ENDFUNCTION()


# Adds includes for a library found with FIND_PACKAGE
FUNCTION(INCLUDE_WITH_VARIABLES TRGTNAME)
	FOREACH(CUR_DEPENDENCY ${ARGN})
		INCLUDE_DIRECTORIES(${${CUR_DEPENDENCY}_INCLUDE_DIRS})
	ENDFOREACH()
ENDFUNCTION()

# Adds includes and link properties for a library found with FIND_PACKAGE
FUNCTION(LINK_WITH_VARIABLES TRGTNAME)
	INCLUDE_WITH_VARIABLES(${TRGTNAME} ${ARGN})
	FOREACH(CUR_DEPENDENCY ${ARGN})
		TARGET_LINK_LIBRARIES(${TRGTNAME} ${${CUR_DEPENDENCY}_LIBRARIES} )
	ENDFOREACH()
ENDFUNCTION()


# Sets the given variable to be the default engine, that is to say the name of the higest priority availaible engine.
MACRO(COMPUTE_DEFAULT_ENGINE PAL_DEFAULT_ENGINE)
	SET(PAL_DEFAULT_ENGINE)
	FOREACH(ENGINE ${PAL_ENGINE_LIST})
		IF(PAL_BUILD_${ENGINE})
			SET(PAL_DEFAULT_ENGINE "${ENGINE}")
		ENDIF()
	ENDFOREACH()
ENDMACRO()

FUNCTION(COUNT_ENGINES OUT_VAR)
	SET(${OUT_VAR} 0)
	FOREACH(ENGINE ${PAL_ENGINE_LIST})
		IF(PAL_BUILD_${ENGINE})
			MATH(EXPR ${OUT_VAR} "${${OUT_VAR}}+1")
		ENDIF()
	ENDFOREACH()
	SET(${OUT_VAR} ${${OUT_VAR}} PARENT_SCOPE)
ENDFUNCTION()

# Calls LINK_WITH_VARIABLES for any available engine (used for PAL_STATIC).
MACRO(INCLUDE_WITH_VARIABLES_ALL_ENGINES TARGET_NAME)
	FOREACH(CUR_ENGINE ${PAL_ENGINE_LIST})
		IF(PAL_BUILD_${CUR_ENGINE} AND ${CUR_ENGINE}_FOUND)
			INCLUDE_WITH_VARIABLES( ${TARGET_NAME} ${CUR_ENGINE} )
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

# Adds a dependency to all built engines (libpal_*)
MACRO(ADD_DEPENDENCY_ALL_ENGINES TARGET_NAME)
	FOREACH(CUR_ENGINE ${PAL_ENGINE_LIST})
		IF(PAL_BUILD_${CUR_ENGINE} AND ${CUR_ENGINE}_FOUND)
			STRING( TOLOWER "${CUR_ENGINE}" ENGINE_LOWER_NAME )
			ADD_DEPENDENCIES( ${TARGET_NAME} "libpal_${ENGINE_LOWER_NAME}" )
		ENDIF()
	ENDFOREACH()
ENDMACRO()

# Adds INTERNAL_DEBUG preprocessor definition if PAL_INTERNAL_DEBUG is set, in debug configurations only.
MACRO(ADD_INTERNAL_DEBUG_DEFINITIONS TARGET_NAME)
	IF(PAL_INTERNAL_DEBUG)
		ADD_TARGET_PROPERTIES(${TARGET_NAME} COMPILE_DEFINITIONS_DEBUG INTERNAL_DEBUG)
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
		IF(${DEPENDENCY}_LIBRARIES OR ${DEPENDENCY}_LIBRARY)
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

			IF(${DEPENDENCY}_LIBRARIES OR ${DEPENDENCY}_LIBRARY)
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


FUNCTION(CONFIGURE_SYSTEM_AOL)
	IF(CMAKE_C_COMPILER_ID STREQUAL "")
		# Unset for MSVC. Don't know why.
		IF(MSVC)
			SET(CMAKE_C_COMPILER_ID "MSVC")
		ENDIF(MSVC)
	ENDIF()
	
	IF(CMAKE_SYSTEM_PROCESSOR STREQUAL "unknown")
		# Unset for CYGWIN. Don't know why. Just try with uname in
		# case it's unset for other OSs where uname -m works, too.
		EXECUTE_PROCESS(COMMAND "uname" "-m"
				RESULT_VARIABLE UNAME_RESULT
				ERROR_VARIABLE UNAME_ERR
				OUTPUT_VARIABLE UNAME_OUT
				OUTPUT_STRIP_TRAILING_WHITESPACE)
		IF(UNAME_RESULT EQUAL 0)
			SET(CMAKE_SYSTEM_PROCESSOR ${UNAME_OUT})
		ENDIF()
	
	ENDIF()
	
	STRING(TOLOWER "${CMAKE_SYSTEM_PROCESSOR}-${CMAKE_SYSTEM_NAME}-${CMAKE_C_COMPILER_ID}"
		TEMP_SYSTEM_AOL)
	SET(SYSTEM_AOL ${TEMP_SYSTEM_AOL}
		CACHE INTERNAL "Detected Architecture-Operating System-Linker types (e.g., i686-cygwin-gnu, x86-windows-msvc), as CMAKE_SYSTEM_PROCESSOR-CMAKE_SYSTEM_NAME-CMAKE_C_COMPILER_ID."
		)
ENDFUNCTION(CONFIGURE_SYSTEM_AOL)

################################################################################
# Set Config file

CONFIGURE_SYSTEM_AOL()
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

			IN_STR_LIST(PAL_VEHICLED_ENGINE_LIST "${CUR_ENGINE}" VEHICLED)
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

COUNT_ENGINES(NB_ENGINES)
IF(PAL_CONFIG_HAS_BEEN_RUN_BEFORE AND ${NB_ENGINES} EQUAL 0)
	MESSAGE("${WARNING_STRING}You didn't select any engine. This may not be what you want...")
ENDIF()
# Temporary warning - To remove once it'll be ok
IF(PAL_STATIC AND ${NB_ENGINES} GREATER 1)
	MESSAGE("${WARNING_STRING}You selected more than one engine whith PAL_STATIC option. This is not supported yet (=it is buggy).")
ENDIF()

ADD_SUBDIRECTORY(pal)		# libpal
ADD_SUBDIRECTORY(pal_i)		# libpal_ode and such
IF(PAL_EXAMPLES_DISPLAY)
	ADD_SUBDIRECTORY(test_lib)	# libtest
	ADD_SUBDIRECTORY(paldemo)	# paldemo
ENDIF()
IF(PAL_EXAMPLES_NO_DISPLAY)
	ADD_SUBDIRECTORY(example)	# palbeginner
ENDIF()
ADD_SUBDIRECTORY(extras)	# collada and scythe examples (Not in a IF statement since it is done in extra/CMakeLists.txt)
ADD_SUBDIRECTORY(pal_benchmark) 	# pal_benchmark (Not in a IF statement since it is done in pal_benchmark/CMakeLists.txt)

IF(TESTING_ENABLED)
	ADD_SUBDIRECTORY(test)	# tests
ENDIF()

# This needs to be run very last so other parts of the scripts can take advantage of this.
IF(NOT PAL_CONFIG_HAS_BEEN_RUN_BEFORE)
	SET(PAL_CONFIG_HAS_BEEN_RUN_BEFORE 1 CACHE INTERNAL "Flag to track whether this is the first time running CMake or if CMake has been configured before")
ENDIF()
