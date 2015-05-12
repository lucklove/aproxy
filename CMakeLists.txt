CMAKE_MINIMUM_REQUIRED(VERSION 2.8.12)
PROJECT(aproxy)

SET(CMAKE_VERBOSE_MAKEFILE ON)
SET(CMAKE_CXX_COMPILER g++)
ADD_COMPILE_OPTIONS("-std=c++1y")
ADD_COMPILE_OPTIONS("-Wall")
SET(APROXY_SOURCES
	src/proxy.cc
	src/package.cc
	src/connection.cc 
	src/response.cc 
	src/request.cc 
	src/parser.cc 
	src/ThreadPool.cc 
	src/server.cc 
	src/RequestHandler.cc 
	src/client.cc
	src/base64.cc
	src/utils.cc
)
ADD_EXECUTABLE(aproxy ${APROXY_SOURCES})
TARGET_LINK_LIBRARIES(aproxy ssl crypto pthread boost_system)
ADD_CUSTOM_TARGET(debug
	COMMAND ${CMAKE_COMMAND} -DCMAKE_BUILD_TYPE=Debug ${CMAKE_SOURCE_DIR}
	COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target all
	COMMENT "Switch CMAKE_BUILD_TYPE to Debug"

)
ADD_CUSTOM_TARGET(release
	COMMAND ${CMAKE_COMMAND} -DCMAKE_BUILD_TYPE=Release ${CMAKE_SOURCE_DIR}
	COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target all
	COMMENT "Switch CMAKE_BUILD_TYPE to Release"
)
