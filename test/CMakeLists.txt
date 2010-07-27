FUNCTION(COMPILE_TEST SOURCEFILE EXE_NAME ENGINE EXTRA_LIBS)
	ADD_EXECUTABLE(
		${EXE_NAME}
		${SOURCEFILE}		
	)

   if (ENGINE)
      INCLUDE_WITH_VARIABLES(${EXE_NAME} ${ENGINE})
   endif ()

	IF(WIN32)
		ADD_TARGET_PROPERTIES(${EXE_NAME} COMPILE_DEFINITIONS NOMINMAX)		# Used for Novodex/PhysX
   ENDIF()

	TARGET_LINK_LIBRARIES( ${EXE_NAME} libpal ${EXTRA_LIBS} ${MATH_LIBRARY} ${DL_LIBRARY} )
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
ENDFUNCTION()

IF(PAL_CONFIG_HAS_BEEN_RUN_BEFORE)
    COMPILE_TEST(bulletRigidLinkTest.cpp bulletRigidLinkTest BULLET libpal_bullet)
    COMPILE_TEST(palBodyTest.cpp palBodyTest "" "")
    COMPILE_TEST(palActivationTest.cpp palActivationTest "" "")
    COMPILE_TEST(palAngularMotorTest.cpp palAngularMotorTest "" "")
ENDIF()


ADD_TEST(palAngularMotorTest env LD_LIBRARY_PATH=${OUTPUT_LIBDIR}:${LD_LIBRARY_PATH} ${OUTPUT_BINDIR}/palAngularMotorTest)
