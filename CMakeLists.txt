project(HazelcastCloud)
cmake_minimum_required(VERSION 2.8.12)
add_definitions("-std=c++11")
include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/client.cpp)
    add_executable(client client.cpp)
    target_link_libraries(client ${CONAN_LIBS})
endif()

if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/client_with_ssl.cpp)
    add_executable(client_with_ssl client_with_ssl.cpp)
    target_link_libraries(client_with_ssl ${CONAN_LIBS})
endif()

set(CMAKE_CXX_FLAGS "-DHZ_BUILD_WITH_SSL")