cmake_minimum_required(VERSION 3.20)
project(alicia-server)

option(BUILD_TESTS "Build tests" ON)

find_package(Boost REQUIRED)

add_executable(alicia-server)

target_sources(alicia-server PRIVATE 
        ${CMAKE_SOURCE_DIR}/src/main.cpp 
        ${CMAKE_SOURCE_DIR}/src/alicia.cpp
)        

target_include_directories(alicia-server PRIVATE 
        ${CMAKE_SOURCE_DIR}/src/include
)

target_compile_features(alicia-server
        PRIVATE cxx_std_20)

target_link_libraries(alicia-server
        PRIVATE Boost::headers)

        
if (BUILD_TESTS) 
        enable_testing()
        add_subdirectory(tests)
endif()