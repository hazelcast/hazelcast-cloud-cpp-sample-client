cmake_minimum_required(VERSION 3.10)

project(HazelcastCloud)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(hazelcast-cpp-client CONFIG REQUIRED)

if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/client.cpp)
    add_executable(client client.cpp)
    target_link_libraries(client PRIVATE hazelcast-cpp-client::hazelcast-cpp-client)
endif()
if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/client_with_ssl.cpp)
    add_executable(client_with_ssl client_with_ssl.cpp)
    target_link_libraries(client_with_ssl PRIVATE hazelcast-cpp-client::hazelcast-cpp-client)
endif()