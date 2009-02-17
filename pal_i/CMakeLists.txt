# CMake rules for each libpal_* modules
# This will use the different engines (ODE, Tokamak, etc.)


# Convinience function to set up a target for libpal_*
FUNCTION(PREPARE_PACKAGE ENGINE_NAME HEADERS_IMPL SOURCE_IMPL HEADERS_BASE SOURCE_BASE HEADERS_FRAMEWORK SOURCE_FRAMEWORK)
	SET(LIB_NAME "libpal_${ENGINE_NAME}")
	STRING( TOUPPER "${ENGINE_NAME}" ENGINE_UPPER_NAME )
	IF(NOT ${ENGINE_UPPER_NAME}_FOUND)
		MESSAGE(STATUS "${WARNING_STRING}${ENGINE_NAME} hasn't been found. Skipping the build of ${LIB_NAME}.")		# SEND_ERROR instead of STATUS?
		SET(PREPARE_PACKAGE_OK FALSE)
	ELSE(NOT ${ENGINE_UPPER_NAME}_FOUND)
		IF(PAL_STATIC)
			SET(PAL_USER_DEFINED_DYNAMIC_OR_STATIC STATIC)
			INCLUDE_WITH_VARIABLES_ALL_ENGINES(${LIB_NAME})
		ELSE(PAL_STATIC)
			SET(PAL_USER_DEFINED_DYNAMIC_OR_STATIC SHARED)
		ENDIF(PAL_STATIC)

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

		#INCLUDE(ModuleInstall OPTIONAL)

		ADD_DEFINITIONS("-DLIBPAL_ODE_EXPORTS")
		# IF(WIN32)
			ADD_DEFINITIONS("-DDLL_GROUP_IMPLEMENTATION")
		# ENDIF(WIN32)

		LINK_WITH_VARIABLES( ${LIB_NAME} ${ENGINE_UPPER_NAME} )
		TARGET_LINK_LIBRARIES( ${LIB_NAME} libpal ${MATH_LIBRARY} ${DL_LIBRARY} )

		IF(NOT BUILD_OUTPUT_DIR_RELEASE_DEBUG)
			# Ugly workaround to remove the "/debug" or "/release" in each output
			SET_TARGET_PROPERTIES(${LIB_NAME} PROPERTIES PREFIX "../")
			SET_TARGET_PROPERTIES(${LIB_NAME} PROPERTIES IMPORT_PREFIX "../")
		ENDIF(NOT BUILD_OUTPUT_DIR_RELEASE_DEBUG)	

		SET(PREPARE_PACKAGE_OK TRUE)
	ENDIF(NOT ${ENGINE_UPPER_NAME}_FOUND)
ENDFUNCTION(PREPARE_PACKAGE ENGINE_NAME HEADERS_IMPL SOURCE_IMPL HEADERS_BASE SOURCE_BASE HEADERS_FRAMEWORK SOURCE_FRAMEWORK)


################################################################################
# BOX2D
IF(PAL_BUILD_BOX2D)
	PREPARE_PACKAGE(box2D "box2d_pal.h" "box2d_pal.cpp" "" "" "" "")
ENDIF(PAL_BUILD_BOX2D)

################################################################################
# BULLET
IF(PAL_BUILD_BULLET)
	PREPARE_PACKAGE(bullet "bullet_pal.h;bullet_palVehicle.h" "bullet_pal.cpp;bullet_palVehicle.cpp" "" "" "" "")
ENDIF(PAL_BUILD_BULLET)

################################################################################
# Havok
IF(PAL_BUILD_HAVOK)
	PREPARE_PACKAGE(havok "havok_pal.h" "havok_pal.cpp" "" "" "" "")
ENDIF(PAL_BUILD_HAVOK)

################################################################################
# IBDS
IF(PAL_BUILD_IBDS)
	PREPARE_PACKAGE(ibds "ibds_pal.h" "ibds_pal.cpp" "" "" "" "")
ENDIF(PAL_BUILD_IBDS)

################################################################################
# JIGGLE
IF(PAL_BUILD_JIGGLE)
	PREPARE_PACKAGE(jiggle "jiggle_pal.h" "jiggle_pal.cpp" "" "" "" "")
ENDIF(PAL_BUILD_JIGGLE)

################################################################################
# NEWTON
IF(PAL_BUILD_NEWTON)
	PREPARE_PACKAGE(newton "newton_pal.h;newton_palVehicle.h" "newton_pal.cpp;newton_palVehicle.cpp" "" "" "" "")
	IF (${PREPARE_PACKAGE_OK})
		MODULE_FILE_COPY(libpal_newton NEWTON)
	ENDIF (${PREPARE_PACKAGE_OK})
ENDIF(PAL_BUILD_NEWTON)

################################################################################
# NOVODEX
IF(PAL_BUILD_NOVODEX)
	PREPARE_PACKAGE(novodex "novodex_pal.h;Stream.h" "novodex_pal.cpp;Stream.cpp" "" "" "" "")
	IF (${PREPARE_PACKAGE_OK})
		MODULE_FILE_COPY(libpal_novodex NOVODEX)
		ADD_DEFINITIONS("-DCORELIB")
		ADD_DEFINITIONS("-DNX32")
		ADD_DEFINITIONS("-DNX_DISABLE_HARDWARE")
		ADD_DEFINITIONS("-DNX_USE_FLUID_API")
		IF(LINUX)
			ADD_DEFINITIONS("-DLINUX")
		ENDIF(LINUX)	
	ENDIF (${PREPARE_PACKAGE_OK})
ENDIF(PAL_BUILD_NOVODEX)

################################################################################
# ODE
IF(PAL_BUILD_ODE)
	PREPARE_PACKAGE(ode "ode_pal.h;hull.h" "ode_pal.cpp;hull.cpp" "" "" "" "")
	IF (${PREPARE_PACKAGE_OK})
		MODULE_FILE_COPY(libpal_ode ODE)
	ENDIF (${PREPARE_PACKAGE_OK})
ENDIF(PAL_BUILD_ODE)

################################################################################
# OPENTISSUE
IF(PAL_BUILD_OPENTISSUE)
	PREPARE_PACKAGE(opentissue "opentissue_pal.h" "opentissue_pal.cpp" "" "" "" "")
	IF (${PREPARE_PACKAGE_OK})
		INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIR})
	ENDIF (${PREPARE_PACKAGE_OK})
ENDIF(PAL_BUILD_OPENTISSUE)

################################################################################
# SPE
IF(PAL_BUILD_SPE)
	PREPARE_PACKAGE(spe "spe_pal.h" "spe_pal.cpp" "" "" "" "")
	IF (${PREPARE_PACKAGE_OK})
		MODULE_FILE_COPY(libpal_spe SPE)
	ENDIF (${PREPARE_PACKAGE_OK})
ENDIF(PAL_BUILD_SPE)

################################################################################
# TRUEAXIS
IF(PAL_BUILD_TRUEAXIS)
	PREPARE_PACKAGE(trueaxis "trueaxis_pal.h" "trueaxis_pal.cpp" "" "" "" "")
ENDIF(PAL_BUILD_TRUEAXIS)

################################################################################
# TOKAMAK
IF(PAL_BUILD_TOKAMAK)
	SET(HEADERS_IMPL "tokamak_pal.h")
	SET(SOURCE_IMPL "tokamak_pal.cpp")
	IF(TOKAMAK_USE_QHULL)
		SET(HEADERS_IMPL ${HEADERS_IMPL} "mFILE.h" "vlen.h")
		SET(SOURCE_IMPL  ${HEADERS_IMPL} "mFILE.c" "vlen.c")
	ENDIF(TOKAMAK_USE_QHULL)

	PREPARE_PACKAGE(tokamak ${HEADERS_IMPL} ${SOURCE_IMPL} "" "" "" "")
	IF (${PREPARE_PACKAGE_OK})
		MODULE_FILE_COPY(libpal_tokamak TOKAMAK)

		ADD_DEFINITIONS("-DTOKAMAK_USE_DLL")
		IF(TOKAMAK_USE_QHULL)
			ADD_DEFINITIONS("-DUSE_QHULL")
		ENDIF(TOKAMAK_USE_QHULL)
	ENDIF (${PREPARE_PACKAGE_OK})
ENDIF(PAL_BUILD_TOKAMAK)
