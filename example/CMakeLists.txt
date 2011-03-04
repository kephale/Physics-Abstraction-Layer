IF(PAL_CONFIG_HAS_BEEN_RUN_BEFORE)

	SET(EXE_NAME palbeginner)

	ADD_EXECUTABLE(
		${EXE_NAME}
		"begin.cpp"
	)

	ADD_DEPENDENCY_ALL_ENGINES(${EXE_NAME})

	#IF(PAL_STATIC)
		ADD_TARGET_PROPERTIES(${EXE_NAME} COMPILE_DEFINITIONS NOMINMAX)		# Used for Novodex/PhysX
	#ENDIF()

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
	
	FUNCTION(ADD_DISPLAY_EXAMPLE _EXE _SOURCE)
		IF(PAL_EXAMPLES_DISPLAY)
			ADD_EXECUTABLE(
				${_EXE}
				${_SOURCE}
				"graphics.cpp"
				"../sdlgl/sdlgl.cpp"
				)
	 		ADD_DEPENDENCY_ALL_ENGINES( ${_EXE} )
			ADD_TARGET_PROPERTIES( ${_EXE} COMPILE_DEFINITIONS NOMINMAX )
			INCLUDE_DIRECTORIES( ${HEADERS_BASE_PATH} )
			TARGET_LINK_LIBRARIES( ${_EXE} libpal ${MATH_LIBRARY} ${DL_LIBRARY} )
			LINK_WITH_VARIABLES( ${_EXE} SDL OPENGL )
			ADD_INTERNAL_DEBUG_DEFINITIONS( ${_EXE} )
			IF(PAL_STATIC)
				LINK_WITH_VARIABLES_ALL_ENGINES(${_EXE})
			ENDIF()
			# Add the postfix to the executable since it is not added automatically as for modules and shared libraries
			SET_TARGET_PROPERTIES(${_EXE} PROPERTIES DEBUG_POSTFIX "${CMAKE_DEBUG_POSTFIX}")
	
			IF(MSVC)
				IF(NOT BUILD_OUTPUT_DIR_RELEASE_DEBUG)
					# Ugly workaround to remove the "/debug" or "/release" in each output
					SET_TARGET_PROPERTIES(${_EXE} PROPERTIES PREFIX "../")
				ENDIF()
			ENDIF()
			
		ENDIF()
	ENDFUNCTION()

	ADD_DISPLAY_EXAMPLE("hingeExample" "hingeExample.cpp")
	ADD_DISPLAY_EXAMPLE("fingersExample" "fingersExample.cpp")
	
ENDIF()
