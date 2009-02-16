SET(LIB_NAME libpal)
SET(PAL_USER_DEFINED_DYNAMIC_OR_STATIC STATIC)		# "SHARED" is for lib+DLL

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

SET(HEADERS_BASE
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

#INCLUDE(ModuleInstall OPTIONAL)

IF(NOT BUILD_OUTPUT_DIR_RELEASE_DEBUG)
	# Ugly workaround to remove the "/debug" or "/release" in each output
	SET_TARGET_PROPERTIES(${LIB_NAME} PROPERTIES PREFIX "../")
	SET_TARGET_PROPERTIES(${LIB_NAME} PROPERTIES IMPORT_PREFIX "../")
ENDIF(NOT BUILD_OUTPUT_DIR_RELEASE_DEBUG)
