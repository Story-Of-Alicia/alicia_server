cmake_minimum_required(VERSION 3.20)
project(alicia-server)

option(BUILD_TESTS "Build tests" ON)

find_package(Boost REQUIRED)

# Project properties interface library
add_library(project-properties INTERFACE)
target_compile_features(project-properties
        INTERFACE cxx_std_20)

# Alicia server library
add_library(alicia-libserver STATIC
        src/libserver/alicia.cpp
        src/libserver/util.cpp
        src/libserver/base/Server.cpp
        src/libserver/command/CommandProtocol.cpp
        src/libserver/command/CommandServer.cpp
        src/libserver/command/lobby/messages.cpp
        src/libserver/command/ranch/messages.cpp
)
target_include_directories(alicia-libserver
        PUBLIC include/)
target_link_libraries(alicia-libserver
        PUBLIC project-properties Boost::headers)

# Alicia server executable
add_executable(alicia-server)
target_sources(alicia-server
        PRIVATE src/server/main.cpp)
target_link_libraries(alicia-server
        PRIVATE project-properties alicia-libserver)

if (BUILD_TESTS) 
        enable_testing()
        add_subdirectory(tests)
endif()