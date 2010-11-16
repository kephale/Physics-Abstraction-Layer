INCLUDE_DIRECTORIES(${GTEST_INCLUDE_DIRS})

FUNCTION(CONFIGURE_TEST SOURCEFILE EXE_NAME ENGINE EXTRA_LIBS)
	ADD_EXECUTABLE(
		${EXE_NAME}
		${SOURCEFILE}
		AbstractPalTest.cpp		
	)

	if (ENGINE)
		INCLUDE_WITH_VARIABLES(${EXE_NAME} ${ENGINE})
	endif ()

	IF(WIN32)
		ADD_TARGET_PROPERTIES(${EXE_NAME} COMPILE_DEFINITIONS NOMINMAX)		# Used for Novodex/PhysX
	ENDIF()

	TARGET_LINK_LIBRARIES( ${EXE_NAME} libpal ${EXTRA_LIBS} ${MATH_LIBRARY} ${DL_LIBRARY} ${GTEST_BOTH_LIBRARIES} )
	ADD_INTERNAL_DEBUG_DEFINITIONS(${EXE_NAME})
	IF(PAL_STATIC)
		LINK_WITH_VARIABLES_ALL_ENGINES(${EXE_NAME})
	ENDIF()

	# Add the postfix to the executable since it is not added automatically as for modules and shared libraries
	SET_TARGET_PROPERTIES(${EXE_NAME} PROPERTIES DEBUG_POSTFIX "${CMAKE_DEBUG_POSTFIX}")

	IF(MSVC)
		IF(NOT BUILD_OUTPUT_DIR_RELEASE_DEBUG)
			# Ugly workaround to remove the "/debug" or "/release" in each output
			SET_TARGET_PROPERTIES(${EXE_NAME} PROPERTIES PREFIX "../")
		ENDIF()
	ENDIF()
	ADD_TEST(NAME "${EXE_NAME}" COMMAND $<TARGET_FILE:${EXE_NAME}>)
	SET_TESTS_PROPERTIES(${EXE_NAME} PROPERTIES ENVIRONMENT PAL_PLUGIN_PATH=${CMAKE_LIBRARY_OUTPUT_DIRECTORY})
ENDFUNCTION()

IF(PAL_CONFIG_HAS_BEEN_RUN_BEFORE)
    CONFIGURE_TEST(bulletRigidLinkTest.cpp bulletRigidLinkTest BULLET libpal_bullet)
    CONFIGURE_TEST(palBodyTest.cpp palBodyTest "" "")
    CONFIGURE_TEST(palActivationTest.cpp palActivationTest "" "")
    CONFIGURE_TEST(palAngularMotorTest.cpp palAngularMotorTest "" "")
# enable this test when spherical links work
#    CONFIGURE_TEST(palSphericalLinkTest.cpp palSphericalLinkTest BULLET libpal_bullet)
    CONFIGURE_TEST(palGenericLinkTest.cpp palGenericLinkTest "" "")
    CONFIGURE_TEST(palLinkFeedbackTest.cpp palLinkFeedbackTest "" "")
ENDIF()
