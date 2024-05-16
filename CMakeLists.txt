cmake_minimum_required(VERSION 3.20)
project(alicia-server)

find_package(Boost REQUIRED)

add_executable(alicia-server
        src/main.cpp)
target_compile_features(alicia-server
        PRIVATE cxx_std_20)
target_link_libraries(alicia-server
        PRIVATE Boost::headers)
