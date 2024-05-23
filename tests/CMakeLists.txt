include(CTest)

add_executable(test_magic)

target_sources(test_magic PRIVATE 
        ${CMAKE_SOURCE_DIR}/tests/test_magic.cpp 
        ${CMAKE_SOURCE_DIR}/src/alicia.cpp
)        

target_include_directories(test_magic PRIVATE 
        ${CMAKE_SOURCE_DIR}/src/include
)

target_compile_features(test_magic
        PRIVATE cxx_std_20)

add_test(NAME TestMagic COMMAND test_magic)

