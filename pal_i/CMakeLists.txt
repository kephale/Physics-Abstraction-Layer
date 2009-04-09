# CMake rules for each libpal_* modules
# This will use the different engines (ODE, Tokamak, etc.)

FUNCTION(ADD_TARGET_PROPERTIES TARGET_NAME PROPERTY_NAME)
	GET_TARGET_PROPERTY(CURRENT_PROPERTY ${TARGET_NAME} ${PROPERTY_NAME})
	IF(CURRENT_PROPERTY)
		SET_TARGET_PROPERTIES(${TARGET_NAME} PROPERTIES ${PROPERTY_NAME} "${CURRENT_PROPERTY};${ARGN}")
	ELSE()
		SET_TARGET_PROPERTIES(${TARGET_NAME} PROPERTIES ${PROPERTY_NAME} "${ARGN}")
	ENDIF()
ENDFUNCTION()

# Convinience function to set up a target for libpal_*
MACRO(PREPARE_PACKAGE ENGINE_NAME HEADERS_IMPL SOURCE_IMPL HEADERS_BASE SOURCE_BASE HEADERS_FRAMEWORK SOURCE_FRAMEWORK)
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
		#INCLUDE_DIRECTORIES(${HEADERS_BASE_PATH})

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

		IF(WIN32)
			ADD_TARGET_PROPERTIES(${LIB_NAME} COMPILE_DEFINITIONS "DLL_GROUP_IMPLEMENTATION")	# "LIBPAL_${ENGINE_UPPER_NAME}_EXPORTS"
		ENDIF()

		ADD_INTERNAL_DEBUG_DEFINITIONS(${LIB_NAME})

		LINK_WITH_VARIABLES( ${LIB_NAME} ${ENGINE_UPPER_NAME} )
		TARGET_LINK_LIBRARIES( ${LIB_NAME} libpal )
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

		SET(LIB_PUBLIC_HEADERS ${HEADERS_FRAMEWORK} ${HEADERS_BASE} ${HEADERS_IMPL})
		INCLUDE(ModuleInstall OPTIONAL)

		SET(PREPARE_PACKAGE_OK TRUE)
	ENDIF()
ENDMACRO()


################################################################################
# BOX2D
IF(PAL_BUILD_BOX2D)
	PREPARE_PACKAGE(box2D "box2d_pal.h" "box2d_pal.cpp" "" "" "" "")
ENDIF()

################################################################################
# BULLET
IF(PAL_BUILD_BULLET)
	PREPARE_PACKAGE(bullet "bullet_pal.h;bullet_palVehicle.h" "bullet_pal.cpp;bullet_palVehicle.cpp" "" "" "" "")
ENDIF()

################################################################################
# Havok
IF(PAL_BUILD_HAVOK)
	PREPARE_PACKAGE(havok "havok_pal.h" "havok_pal.cpp" "" "" "" "")
ENDIF()

################################################################################
# IBDS
IF(PAL_BUILD_IBDS)
	PREPARE_PACKAGE(ibds "ibds_pal.h" "ibds_pal.cpp" "" "" "" "")
ENDIF()

################################################################################
# JIGGLE
IF(PAL_BUILD_JIGGLE)
	PREPARE_PACKAGE(jiggle "jiggle_pal.h" "jiggle_pal.cpp" "" "" "" "")
ENDIF()

################################################################################
# NEWTON
IF(PAL_BUILD_NEWTON)
	PREPARE_PACKAGE(newton "newton_pal.h;newton_palVehicle.h" "newton_pal.cpp;newton_palVehicle.cpp" "" "" "" "")
	IF (PREPARE_PACKAGE_OK)
		MODULE_FILE_COPY(libpal_newton NEWTON)
	ENDIF (PREPARE_PACKAGE_OK)
ENDIF()

################################################################################
# NOVODEX
IF(PAL_BUILD_NOVODEX)
	PREPARE_PACKAGE(novodex "novodex_pal.h;Stream.h" "novodex_pal.cpp;Stream.cpp" "" "" "" "")
	IF (PREPARE_PACKAGE_OK)
		MODULE_FILE_COPY(libpal_novodex NOVODEX)
		ADD_TARGET_PROPERTIES(libpal_novodex COMPILE_DEFINITIONS "CORELIB" "NX32" "NX_DISABLE_HARDWARE")
		IF(LINUX)
			ADD_TARGET_PROPERTIES(libpal_novodex COMPILE_DEFINITIONS "NX_DISABLE_FLUIDS" "LINUX")
		ELSE()
			ADD_TARGET_PROPERTIES(libpal_novodex COMPILE_DEFINITIONS "NX_USE_FLUID_API")
		ENDIF()	
	ENDIF (PREPARE_PACKAGE_OK)
ENDIF()

################################################################################
# ODE
IF(PAL_BUILD_ODE)
	PREPARE_PACKAGE(ode "ode_pal.h;hull.h" "ode_pal.cpp;hull.cpp" "" "" "" "")
	IF (PREPARE_PACKAGE_OK)
		MODULE_FILE_COPY(libpal_ode ODE)
	ENDIF (PREPARE_PACKAGE_OK)
ENDIF()

################################################################################
# OPENTISSUE
IF(PAL_BUILD_OPENTISSUE)
	PREPARE_PACKAGE(opentissue "opentissue_pal.h" "opentissue_pal.cpp" "" "" "" "")
	IF (PREPARE_PACKAGE_OK)
		INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS})
	ENDIF (PREPARE_PACKAGE_OK)
ENDIF()

################################################################################
# SPE
IF(PAL_BUILD_SPE)
	PREPARE_PACKAGE(spe "spe_pal.h" "spe_pal.cpp" "" "" "" "")
	IF (PREPARE_PACKAGE_OK)
		MODULE_FILE_COPY(libpal_spe SPE)
	ENDIF (PREPARE_PACKAGE_OK)
ENDIF()

################################################################################
# TRUEAXIS
IF(PAL_BUILD_TRUEAXIS)
	PREPARE_PACKAGE(trueaxis "trueaxis_pal.h" "trueaxis_pal.cpp" "" "" "" "")
ENDIF()

################################################################################
# TOKAMAK
IF(PAL_BUILD_TOKAMAK)
	SET(HEADERS_IMPL "tokamak_pal.h")
	SET(SOURCE_IMPL "tokamak_pal.cpp")
	IF(TOKAMAK_USE_QHULL)
		SET(HEADERS_IMPL ${HEADERS_IMPL} "mFILE.h" "vlen.h")
		SET(SOURCE_IMPL  ${HEADERS_IMPL} "mFILE.c" "vlen.c")
	ENDIF()

	PREPARE_PACKAGE(tokamak ${HEADERS_IMPL} ${SOURCE_IMPL} "" "" "" "")
	IF (PREPARE_PACKAGE_OK)
		MODULE_FILE_COPY(libpal_tokamak TOKAMAK)

		ADD_TARGET_PROPERTIES(libpal_tokamak COMPILE_DEFINITIONS "TOKAMAK_USE_DLL")
		IF(TOKAMAK_USE_QHULL)
			ADD_TARGET_PROPERTIES(libpal_tokamak COMPILE_DEFINITIONS "USE_QHULL")
		ENDIF()
	ENDIF (PREPARE_PACKAGE_OK)
ENDIF()
