### PacketBuffer Tests ###
add_executable(PacketBuffer_unit_tests
    PacketBuffer_unit_tests.cpp
    ${NC_SOURCE_DIR}/network/PacketBuffer.cpp
)

target_include_directories(PacketBuffer_unit_tests
    PRIVATE
        ${NC_INCLUDE_DIR}
        ${NC_SOURCE_DIR}
        ${NC_EXTERNAL_DIR}/gtest/googletest/include
)

target_compile_options(PacketBuffer_unit_tests
    PUBLIC
        ${NC_COMPILER_FLAGS}
)

target_link_libraries(PacketBuffer_unit_tests
    PRIVATE
        gtest
)

add_test(PacketBuffer_unit_tests PacketBuffer_unit_tests)