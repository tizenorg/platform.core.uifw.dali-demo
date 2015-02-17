SET(DEMO_SRC_DIR ${ROOT_SRC_DIR}/demo)

AUX_SOURCE_DIRECTORY(${DEMO_SRC_DIR} DEMO_SRCS)

INCLUDE_DIRECTORIES(${DEMO_SRC_DIR})

ADD_EXECUTABLE(${PROJECT_NAME} ${DEMO_SRCS})

#get_cmake_property(_variableNames VARIABLES)
#foreach (_variableName ${_variableNames})
#    message(STATUS "${_variableName}=${${_variableName}}")
#endforeach()
