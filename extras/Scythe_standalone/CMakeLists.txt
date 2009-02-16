SET(EXE_NAME Scythe_StandAlone)

SET(HEADERS
	physLoadSave.h
	ScythePhysics.h
	Vector3.h
)

SET(SOURCE
	main.cpp
	physload.cpp
	ScythePhysics.cpp
)

ADD_EXECUTABLE(
	${EXE_NAME}
	${HEADERS}
	${SOURCE}
)

TARGET_LINK_LIBRARIES( ${EXE_NAME} libpal libtest )
LINK_WITH_VARIABLES( ${EXE_NAME} SDL OPENGL )

# Add the postfix to the executable since it is not added automatically as for modules and shared libraries
SET_TARGET_PROPERTIES(${EXE_NAME} PROPERTIES DEBUG_POSTFIX "${CMAKE_DEBUG_POSTFIX}")

IF(NOT BUILD_OUTPUT_DIR_RELEASE_DEBUG)
	# Ugly workaround to remove the "/debug" or "/release" in each output
	SET_TARGET_PROPERTIES(${EXE_NAME} PROPERTIES PREFIX "../")
ENDIF(NOT BUILD_OUTPUT_DIR_RELEASE_DEBUG)
