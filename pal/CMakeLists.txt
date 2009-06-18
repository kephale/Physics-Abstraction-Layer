SET(LIB_NAME libpal)
IF(PAL_STATIC)
	SET(PAL_USER_DEFINED_DYNAMIC_OR_STATIC STATIC)
ELSE()
	IF(MSVC OR APPLE)
		# NOT YET SUPPORTED (missing exports under MSVC)???
		SET(PAL_USER_DEFINED_DYNAMIC_OR_STATIC STATIC)
	ELSE()  # on linux it must be compiled shared or there are symbol issues at runtime
		SET(PAL_USER_DEFINED_DYNAMIC_OR_STATIC SHARED)
	ENDIF()
	# #ADD_DEFINITIONS(-D_USRDLL -DDLL_EXPORTS)
ENDIF()

SET(HEADERS_FRAMEWORK
	${HEADERS_FRAMEWORK_PATH}/common.h
	${HEADERS_FRAMEWORK_PATH}/empty.h
	${HEADERS_FRAMEWORK_PATH}/errorlog.h
	${HEADERS_FRAMEWORK_PATH}/factory.h
	${HEADERS_FRAMEWORK_PATH}/factoryconfig.h
	${HEADERS_FRAMEWORK_PATH}/managedmemoryobject.h
	${HEADERS_FRAMEWORK_PATH}/os.h
	${HEADERS_FRAMEWORK_PATH}/osfs.h
	${HEADERS_FRAMEWORK_PATH}/statuscode.h
	${HEADERS_FRAMEWORK_PATH}/statusobject.h
)

SET(SOURCE_FRAMEWORK
	${SOURCE_FRAMEWORK_PATH}/errorlog.cpp
	${SOURCE_FRAMEWORK_PATH}/factoryconfig.cpp
	${SOURCE_FRAMEWORK_PATH}/osfs.cpp
	${SOURCE_FRAMEWORK_PATH}/statusobject.cpp
)
SOURCE_GROUP("framework" FILES ${HEADERS_FRAMEWORK})
SOURCE_GROUP("framework" FILES ${SOURCE_FRAMEWORK})
INCLUDE_DIRECTORIES(${HEADERS_FRAMEWORK_PATH})
IF(PAL_STATIC)
	INCLUDE_WITH_VARIABLES_ALL_ENGINES(${LIB_NAME})
ENDIF()

SET(HEADERS_BASE
	${PAL_CONFIG_HEADER}
	${PAL_CONFIG_STATIC_HEADER}
	ConfigVersion.h
	pal.h
	palActuators.h
	palBase.h
	palBodies.h
	palBodyBase.h
	palCollision.h
	palExtraActuators.h
	palFactory.h
	palFluid.h
	palGeometry.h
	palLinks.h
	palMaterials.h
	palMath.h
	palSensors.h
	palSettings.h
	palSolver.h
	palStatic.h
	palTerrain.h
	palVehicle.h
)
SET(SOURCE_BASE
	pal.cpp
	palActuators.cpp
	palBodies.cpp
	palBodyBase.cpp
	palCollision.cpp
	palFactory.cpp
	palFluid.cpp
	palGeometry.cpp
	palLinks.cpp
	palMath.cpp
	palSensors.cpp
	palSolver.cpp
	palStatic.cpp
	palSoftBody.cpp
	palTerrain.cpp
)
SOURCE_GROUP("pal" FILES ${HEADERS_BASE})
SOURCE_GROUP("pal" FILES ${SOURCE_BASE})
#INCLUDE_DIRECTORIES(${HEADERS_BASE_PATH})

SET(HEADERS_IMPL
	${HEADERS_IMPL_PATH}/hull.h
)
SET(SOURCE_IMPL
	${SOURCE_IMPL_PATH}/hull.cpp
)
SOURCE_GROUP("pal_i" FILES ${HEADERS_IMPL})
SOURCE_GROUP("pal_i" FILES ${SOURCE_IMPL})
INCLUDE_DIRECTORIES(${HEADERS_IMPL_PATH})

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

# Remove the "lib" prefix since the library is already named "libpal".
SET_TARGET_PROPERTIES(${LIB_NAME} PROPERTIES PREFIX "")
SET_TARGET_PROPERTIES(${LIB_NAME} PROPERTIES IMPORT_PREFIX "")

TARGET_LINK_LIBRARIES( ${LIB_NAME} ${MATH_LIBRARY} ${DL_LIBRARY} )
IF(UNIX)
	TARGET_LINK_LIBRARIES( ${LIB_NAME} ${MATH_LIBRARY} ${DL_LIBRARY} ${CMAKE_THREAD_LIBS_INIT} )
ENDIF()
ADD_INTERNAL_DEBUG_DEFINITIONS(${LIB_NAME})

IF(MSVC_IDE)
	IF(NOT BUILD_OUTPUT_DIR_RELEASE_DEBUG)
		# Ugly workaround to remove the "/debug" or "/release" in each output
		SET_TARGET_PROPERTIES(${LIB_NAME} PROPERTIES PREFIX "../")
		SET_TARGET_PROPERTIES(${LIB_NAME} PROPERTIES IMPORT_PREFIX "../")
	ENDIF()
ENDIF()

INCLUDE(ModuleInstall OPTIONAL)
