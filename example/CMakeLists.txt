IF(PAL_CONFIG_HAS_BEEN_RUN_BEFORE)

	SET(EXE_NAME palbeginner)

	ADD_EXECUTABLE(
		${EXE_NAME}
		"begin.cpp"
	)

	ADD_DEPENDENCY_ALL_ENGINES(${EXE_NAME})

	TARGET_LINK_LIBRARIES( ${EXE_NAME} libpal ${MATH_LIBRARY} ${DL_LIBRARY} )
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
	
ENDIF()
