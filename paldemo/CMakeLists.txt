IF(NOT MSVC)
	MESSAGE (STATUS "${WARNING_STRING}PAL Demo is only available on Win32/MSVC.")
ELSE(NOT MSVC)

SET(EXE_NAME paldemo)

SET(HEADERS
	test.h
	test1.h
	test2.h
	test3.h
	test4.h
	test5.h
	test6.h
	test_collision.h
	test_motor.h
	test_water.h
)

SET(SOURCE
	test.cpp
	test1.cpp
	test2.cpp
	test3.cpp
	test4.cpp
	test5.cpp
	test6.cpp
	test_collision.cpp
	test_motor.cpp
	test_water.cpp
	testmain.cpp
)

IF(MSVC)
	SET(SOURCE ${SOURCE} res.rc)
ENDIF(MSVC)

ADD_EXECUTABLE(
	${EXE_NAME}
	${HEADERS}
	${SOURCE}
)

TARGET_LINK_LIBRARIES( ${EXE_NAME} libpal libtest ${MATH_LIBRARY} ${DL_LIBRARY} )
LINK_WITH_VARIABLES( ${EXE_NAME} SDL OPENGL )
IF(PAL_STATIC)
	LINK_WITH_VARIABLES_ALL_ENGINES(${EXE_NAME})
ENDIF(PAL_STATIC)

# Add the postfix to the executable since it is not added automatically as for modules and shared libraries
SET_TARGET_PROPERTIES(${EXE_NAME} PROPERTIES DEBUG_POSTFIX "${CMAKE_DEBUG_POSTFIX}")

IF(NOT BUILD_OUTPUT_DIR_RELEASE_DEBUG)
	# Ugly workaround to remove the "/debug" or "/release" in each output
	SET_TARGET_PROPERTIES(${EXE_NAME} PROPERTIES PREFIX "../")
ENDIF(NOT BUILD_OUTPUT_DIR_RELEASE_DEBUG)


MODULE_FILE_COPY(paldemo SDL)

ENDIF(NOT MSVC)