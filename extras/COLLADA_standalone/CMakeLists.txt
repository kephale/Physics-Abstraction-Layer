SET(EXE_NAME COLLADA_StandAlone)

SET(HEADERS
	DAE2XML_Asc2Bin.h
	DAE2XML_ColladaPhysics.h
	DAE2XML_hull.h
	DAE2XML_tinystr.h
	DAE2XML_tinyxml.h
)

SET(SOURCE
	DAE2XML.cpp
	DAE2XML_Asc2Bin.cpp
	DAE2XML_ColladaPhysics.cpp
	DAE2XML_hull.cpp
	DAE2XML_tinystr.cpp
	DAE2XML_tinyxml.cpp
	DAE2XML_tinyxmlerror.cpp
	DAE2XML_tinyxmlparser.cpp
)

ADD_EXECUTABLE(
	${EXE_NAME}
	${HEADERS}
	${SOURCE}
)

TARGET_LINK_LIBRARIES( ${EXE_NAME} libpal libtest ${MATH_LIBRARY} ${DL_LIBRARY} )
LINK_WITH_VARIABLES( ${EXE_NAME} SDL OPENGL )
ADD_INTERNAL_DEBUG_DEFINITIONS(${EXE_NAME})
IF(PAL_STATIC)
	LINK_WITH_VARIABLES_ALL_ENGINES(${EXE_NAME})
ENDIF()

# Add the postfix to the executable since it is not added automatically as for modules and shared libraries
SET_TARGET_PROPERTIES(${EXE_NAME} PROPERTIES DEBUG_POSTFIX "${CMAKE_DEBUG_POSTFIX}")

IF(MSVC_IDE)
	IF(NOT BUILD_OUTPUT_DIR_RELEASE_DEBUG)
		# Ugly workaround to remove the "/debug" or "/release" in each output
		SET_TARGET_PROPERTIES(${EXE_NAME} PROPERTIES PREFIX "../")
	ENDIF()
ENDIF()
	