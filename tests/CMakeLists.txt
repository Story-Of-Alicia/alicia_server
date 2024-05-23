include(CTest)

add_executable(test_magic)

target_sources(test_magic PRIVATE 
        ${CMAKE_SOURCE_DIR}/tests/test_magic.cpp)
target_link_libraries(test_magic
        PRIVATE project-properties alicia-libserver)

add_test(NAME TestMagic COMMAND test_magic)

