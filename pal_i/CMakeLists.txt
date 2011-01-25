# CMake rules for each libpal_* modules
# This will use the different engines (ODE, Tokamak, etc.)

# Convinience function to set up a target for libpal_*.
# Additional arguments will be treated as additional libraries to link
# against (for engines that have more than one dynamic library).
MACRO(PREPARE_PACKAGE ENGINE_NAME HEADERS_IMPL SOURCE_IMPL HEADERS_BASE SOURCE_BASE HEADERS_FRAMEWORK SOURCE_FRAMEWORK )
	SET(LIB_NAME "libpal_${ENGINE_NAME}")
	STRING( TOUPPER "${ENGINE_NAME}" ENGINE_UPPER_NAME )
	IF(NOT ${ENGINE_UPPER_NAME}_FOUND)
		MESSAGE(STATUS "${WARNING_STRING}${ENGINE_NAME} hasn't been found. Skipping the build of ${LIB_NAME}.")		# SEND_ERROR instead of STATUS?
		SET(PREPARE_PACKAGE_OK FALSE)
	ELSE()
		IF(PAL_STATIC)
			SET(PAL_USER_DEFINED_DYNAMIC_OR_STATIC STATIC)
			INCLUDE_WITH_VARIABLES_ALL_ENGINES(${LIB_NAME})
		ELSE()
			SET(PAL_USER_DEFINED_DYNAMIC_OR_STATIC SHARED)
		ENDIF()

		SOURCE_GROUP("framework" FILES ${HEADERS_FRAMEWORK})
		SOURCE_GROUP("framework" FILES ${SOURCE_FRAMEWORK})
		INCLUDE_DIRECTORIES(${HEADERS_FRAMEWORK_PATH})

		SOURCE_GROUP("pal" FILES ${HEADERS_BASE})
		SOURCE_GROUP("pal" FILES ${SOURCE_BASE})
		INCLUDE_DIRECTORIES(${HEADERS_BASE_PATH})

		SOURCE_GROUP("pal_i" FILES ${HEADERS_IMPL})
		SOURCE_GROUP("pal_i" FILES ${SOURCE_IMPL})
		#INCLUDE_DIRECTORIES(${HEADERS_IMPL_PATH})

		ADD_LIBRARY(
			${LIB_NAME}
			${PAL_USER_DEFINED_DYNAMIC_OR_STATIC}
			${HEADERS_FRAMEWORK}
			${SOURCE_FRAMEWORK}
			${HEADERS_BASE}
			${SOURCE_BASE}
			${HEADERS_IMPL}
			${SOURCE_IMPL}
		)

		# Remove the "lib" prefix since the library is already named "libpal_*".
		SET_TARGET_PROPERTIES(${LIB_NAME} PROPERTIES PREFIX "")
		SET_TARGET_PROPERTIES(${LIB_NAME} PROPERTIES IMPORT_PREFIX "")

		IF(NOT PAL_STATIC AND WIN32)
			  ADD_TARGET_PROPERTIES(${LIB_NAME} COMPILE_DEFINITIONS "DLL_GROUP_IMPLEMENTATION")	# "LIBPAL_${ENGINE_UPPER_NAME}_EXPORTS"
		ENDIF()

		ADD_INTERNAL_DEBUG_DEFINITIONS(${LIB_NAME})

		LINK_WITH_VARIABLES( ${LIB_NAME} ${ENGINE_UPPER_NAME} )
		TARGET_LINK_LIBRARIES( ${LIB_NAME} libpal ${ARGN} )
		IF(UNIX)
			TARGET_LINK_LIBRARIES( ${LIB_NAME} ${MATH_LIBRARY} ${DL_LIBRARY} ${CMAKE_THREAD_LIBS_INIT} )
		ENDIF()

		IF(MSVC_IDE)
			IF(NOT BUILD_OUTPUT_DIR_RELEASE_DEBUG)
				# Ugly workaround to remove the "/debug" or "/release" in each output
				SET_TARGET_PROPERTIES(${LIB_NAME} PROPERTIES PREFIX "../")
				SET_TARGET_PROPERTIES(${LIB_NAME} PROPERTIES IMPORT_PREFIX "../")
			ENDIF()	
		ENDIF()

		INCLUDE(ModuleInstall OPTIONAL)

		ADD_TARGET_PROPERTIES(${LIB_NAME} INSTALL_RPATH ${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBDIR})

		SET(PREPARE_PACKAGE_OK TRUE)
	ENDIF()
ENDMACRO()


################################################################################
# BOX2D
IF(PAL_BUILD_BOX2D)
	PREPARE_PACKAGE(box2d "box2d/box2d_pal.h" "box2d/box2d_pal.cpp" "" "" "" "" "")		# Not box2D ('D' in caps) because of ADD_DEPENDENCY_ALL_ENGINES() macro.
ENDIF()

################################################################################
# BULLET
IF(PAL_BUILD_BULLET)
	PREPARE_PACKAGE(bullet "bullet/bullet_pal.h;bullet/bullet_palVehicle.h;bullet/bullet_palCharacter.h;bullet/bullet_palHingeConstraint.h" "bullet/bullet_pal.cpp;bullet/bullet_palVehicle.cpp;bullet/bullet_palCharacter.cpp;bullet/bullet_palHingeConstraint.cpp" "" "" "" "" ${BULLET_LIBRARIES} )
ENDIF()

################################################################################
# Havok
IF(PAL_BUILD_HAVOK)
	PREPARE_PACKAGE(havok "havok/havok_pal.h" "havok/havok_pal.cpp" "" "" "" "")
ENDIF()

################################################################################
# IBDS
IF(PAL_BUILD_IBDS)
	PREPARE_PACKAGE(ibds "ibds/ibds_pal.h" "ibds/ibds_pal.cpp" "" "" "" "")
ENDIF()

################################################################################
# JIGGLE
IF(PAL_BUILD_JIGGLE)
	PREPARE_PACKAGE(jiggle "jiggle/jiggle_pal.h" "jiggle/jiggle_pal.cpp" "" "" "" "")
ENDIF()

################################################################################
# NEWTON
IF(PAL_BUILD_NEWTON)
	PREPARE_PACKAGE(newton "newton/newton_pal.h;newton/newton_palVehicle.h" "newton/newton_pal.cpp;newton/newton_palVehicle.cpp" "" "" "" "")
	IF (PREPARE_PACKAGE_OK)
		MODULE_FILE_COPY(libpal_newton NEWTON)
	ENDIF (PREPARE_PACKAGE_OK)
ENDIF()

################################################################################
# NOVODEX
IF(PAL_BUILD_NOVODEX)
	PREPARE_PACKAGE(novodex "novodex/novodex_pal.h;novodex/Stream.h" "novodex/novodex_pal.cpp;novodex/Stream.cpp" "" "" "" "")
	IF (PREPARE_PACKAGE_OK)
		MODULE_FILE_COPY(libpal_novodex NOVODEX)
		ADD_TARGET_PROPERTIES(libpal_novodex COMPILE_DEFINITIONS "CORELIB" "NX32" "NX_DISABLE_HARDWARE")
		IF(${CMAKE_SYSTEM} MATCHES "Linux")
			ADD_TARGET_PROPERTIES(libpal_novodex COMPILE_DEFINITIONS "NX_DISABLE_FLUIDS" "LINUX")
		ELSE()
			ADD_TARGET_PROPERTIES(libpal_novodex COMPILE_DEFINITIONS "NX_USE_FLUID_API")
		ENDIF()	
	ENDIF (PREPARE_PACKAGE_OK)
ENDIF()

################################################################################
# ODE
IF(PAL_BUILD_ODE)
	PREPARE_PACKAGE(ode "ode/ode_pal.h;hull.h" "ode/ode_pal.cpp;hull.cpp" "" "" "" "")
	IF (PREPARE_PACKAGE_OK)
		MODULE_FILE_COPY(libpal_ode ODE)
	ENDIF (PREPARE_PACKAGE_OK)
ENDIF()

################################################################################
# OPENTISSUE
IF(PAL_BUILD_OPENTISSUE)
	PREPARE_PACKAGE(opentissue "opentissue/opentissue_pal.h" "opentissue/opentissue_pal.cpp" "" "" "" "")
	IF (PREPARE_PACKAGE_OK)
		INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS})
	ENDIF (PREPARE_PACKAGE_OK)
ENDIF()

################################################################################
# SPE
IF(PAL_BUILD_SPE)
	PREPARE_PACKAGE(spe "spe/spe_pal.h" "spe/spe_pal.cpp" "" "" "" "")
	IF (PREPARE_PACKAGE_OK)
		MODULE_FILE_COPY(libpal_spe SPE)
	ENDIF (PREPARE_PACKAGE_OK)
ENDIF()

################################################################################
# TRUEAXIS
IF(PAL_BUILD_TRUEAXIS)
	PREPARE_PACKAGE(trueaxis "trueaxis/trueaxis_pal.h" "trueaxis/trueaxis_pal.cpp" "" "" "" "")
ENDIF()

################################################################################
# TOKAMAK
IF(PAL_BUILD_TOKAMAK)
	SET(HEADERS_IMPL "tokamak/tokamak_pal.h")
	SET(SOURCE_IMPL "tokamak/tokamak_pal.cpp")
	IF(TOKAMAK_USE_QHULL)
		SET(HEADERS_IMPL ${HEADERS_IMPL} "mFILE.h" "vlen.h")
		SET(SOURCE_IMPL  ${HEADERS_IMPL} "mFILE.c" "vlen.c")
	ENDIF()

	PREPARE_PACKAGE(tokamak ${HEADERS_IMPL} ${SOURCE_IMPL} "" "" "" "")
	IF (PREPARE_PACKAGE_OK)
		MODULE_FILE_COPY(libpal_tokamak TOKAMAK)

		IF (WIN32)
			ADD_TARGET_PROPERTIES(libpal_tokamak COMPILE_DEFINITIONS "TOKAMAK_USE_DLL")
		ENDIF()
		IF(TOKAMAK_USE_QHULL)
			ADD_TARGET_PROPERTIES(libpal_tokamak COMPILE_DEFINITIONS "USE_QHULL")
		ENDIF()
	ENDIF (PREPARE_PACKAGE_OK)
ENDIF()
