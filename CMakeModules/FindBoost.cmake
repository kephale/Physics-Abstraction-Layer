# Extension of the standard FindBost.cmake
# Adds Boost_${COMPONENT}_LIBRARIES and Boost_${COMPONENT}_INCLUDE_DIRS variables
INCLUDE("${CMAKE_ROOT}/Modules/FindBoost.cmake")

FOREACH(COMPONENT ${Boost_FIND_COMPONENTS})
	STRING(TOUPPER ${COMPONENT} COMPONENT)
	#IF(Boost_${COMPONENT}_FOUND)		# After all, there is no harm setting the variables when the component hasn't been found.
		SET(Boost_${COMPONENT}_LIBRARIES ${Boost_${COMPONENT}_LIBRARY})
		SET(Boost_${COMPONENT}_INCLUDE_DIRS ${Boost_INCLUDE_DIRS})
	#ENDIF()
ENDFOREACH()
