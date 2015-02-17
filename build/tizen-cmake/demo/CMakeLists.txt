SET(DEMO_SRC_DIR ${ROOT_SRC_DIR}/demo)

SET(PKG_LIST dali
             dali-toolkit)

INCLUDE(FindPkgConfig)
pkg_check_modules(REQUIRED_PKGS REQUIRED ${PKG_LIST})

FOREACH(flag ${REQUIRED_PKGS_CFLAGS})
	SET(REQUIRED_CFLAGS "${REQUIRED_CFLAGS} ${flag}")
ENDFOREACH(flag)

SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${REQUIRED_CFLAGS} -Werror -Wall")
SET(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS}")

AUX_SOURCE_DIRECTORY(${DEMO_SRC_DIR} DEMO_SRCS)

INCLUDE_DIRECTORIES(${ROOT_SRC_DIR})
INCLUDE_DIRECTORIES(${DEMO_SRC_DIR})

ADD_EXECUTABLE(${PROJECT_NAME} ${DEMO_SRCS})

#get_cmake_property(_variableNames VARIABLES)
#foreach (_variableName ${_variableNames})
#    message(STATUS "${_variableName}=${${_variableName}}")
#endforeach()
