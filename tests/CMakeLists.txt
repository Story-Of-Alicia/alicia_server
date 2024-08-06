include(CTest)

add_executable(test_magic)
target_sources(test_magic PRIVATE ${CMAKE_SOURCE_DIR}/tests/test_magic.cpp)
target_link_libraries(test_magic PRIVATE project-properties alicia-libserver)
add_test(NAME TestMagic COMMAND test_magic)

add_executable(test_packet)
target_sources(test_packet PRIVATE ${CMAKE_SOURCE_DIR}/tests/test_packet.cpp)
target_link_libraries(test_packet PRIVATE project-properties alicia-libserver)
target_compile_definitions(test_packet PUBLIC ALICIA_PROTOCOL=1)
add_test(NAME TestPacket COMMAND test_packet)

