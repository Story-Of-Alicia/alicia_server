cmake_minimum_required(VERSION 3.20)
project(alicia-server)

option(BUILD_TESTS "Build tests" ON)

find_package(Boost REQUIRED)

# Project properties interface library
add_library(project-properties INTERFACE)
target_compile_features(project-properties
        INTERFACE cxx_std_20)

# Alicia server library
add_library(alicia-libserver STATIC)
target_sources(alicia-libserver
        PUBLIC src/libserver/alicia.cpp  src/libserver/mapping.cpp)
target_include_directories(alicia-libserver
        PUBLIC include/)
target_link_libraries(alicia-libserver
        PUBLIC project-properties Boost::headers)

# Alicia 1.0 server executable
add_executable(alicia1-server)
target_sources(alicia1-server
        PRIVATE src/server/main.cpp)
target_link_libraries(alicia1-server
        PRIVATE project-properties alicia-libserver)
target_compile_definitions(alicia1-server PUBLIC ALICIA_PROTOCOL=1)

# Alicia 2.0 server executable
add_executable(alicia2-server)
target_sources(alicia2-server
        PRIVATE src/server/main.cpp)
target_link_libraries(alicia2-server
        PRIVATE project-properties alicia-libserver)
target_compile_definitions(alicia2-server PUBLIC ALICIA_PROTOCOL=2)


if (BUILD_TESTS) 
        enable_testing()
        add_subdirectory(tests)
endif()