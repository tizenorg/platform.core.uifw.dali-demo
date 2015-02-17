SET(EXAMPLES_SRC_DIR ${ROOT_SRC_DIR}/examples)

SET(BLOCKS_SRCS ${EXAMPLES_SRC_DIR}/blocks/blocks-example.cpp)
ADD_EXECUTABLE(blocks.example ${BLOCKS_SRCS})
TARGET_LINK_LIBRARIES(blocks.example ${REQUIRED_PKGS_LDFLAGS})
INSTALL(TARGETS blocks.example DESTINATION ${BINDIR})

#get_cmake_property(_variableNames VARIABLES)
#foreach (_variableName ${_variableNames})
#    message(STATUS "${_variableName}=${${_variableName}}")
#endforeach()
