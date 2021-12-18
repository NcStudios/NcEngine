### PacketBuffer Tests ###
add_executable(PacketBuffer_unit_tests
    ${ROOT_DIR}/test/unit/network/PacketBuffer_unit_tests.cpp
    ${ROOT_DIR}/source/network/PacketBuffer.cpp
)

target_include_directories(PacketBuffer_unit_tests
    PRIVATE
        ${NC_INCLUDE_DIR}
        ${NC_SOURCE_DIR}
        ${ROOT_DIR}/external/gtest/googletest/include
)

target_compile_options(PacketBuffer_unit_tests
    PUBLIC
        ${GCC_WARNING_FLAGS}
)

target_link_libraries(PacketBuffer_unit_tests
    PRIVATE
        gtest
)

add_test(PacketBuffer_unit_tests PacketBuffer_unit_tests)