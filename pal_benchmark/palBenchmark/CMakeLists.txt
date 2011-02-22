SET(EXE_NAME palBenchmark)

SET(HEADERS
	bridge_scene.h
	collision_scene.h
	dialog.h
	drop_scene.h
	graph.h
	main.h
	paltest.h
	resource.h
	restitution_scene.h
	stack_ball_scene.h
	stack_scene.h
	standard_scenes.h
	strings.h
	"../../sdlgl/sdlgl.h"
	"../../example/graphics.h"
)

SET(SOURCE
	main.cpp
)

# WIN32 is ignored on non-Windows platforms, but is required to link
# properly on Windows. For details, see:
# http://www.cmake.org/Wiki/CMake_FAQ#Why_am_I_getting_a_linker_error_to__mainCRTStartup_under_WIN32.3F

ADD_EXECUTABLE(
	${EXE_NAME}
	WIN32
	${HEADERS}
	${SOURCE}
)

INCLUDE_DIRECTORIES(${HEADERS_BASE_PATH})

find_package(OpenGL)
TARGET_LINK_LIBRARIES( ${EXE_NAME} libpal libtest ${MATH_LIBRARY} ${DL_LIBRARY} )
INCLUDE_WITH_VARIABLES( ${EXE_NAME} IRRLICHT OPENGL SDL )
LINK_WITH_VARIABLES( ${EXE_NAME} IRRLICHT SDL OPENGL )
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
