### Entity Tests ###
add_executable(Entity_unit_tests
    ${ROOT_DIR}/test/unit/Entity_unit_tests.cpp
    ${ROOT_DIR}/nc/source/component/Component.cpp
    ${ROOT_DIR}/nc/source/ecs/Entity.cpp
)

target_include_directories(Entity_unit_tests
    PRIVATE
        ${NC_INCLUDE_DIRS}
        ${EXTERNAL_INCLUDE_DIR}
        ${ROOT_DIR}/external/gtest/googletest/include
)

target_compile_options(Entity_unit_tests
    PUBLIC
        ${GCC_WARNING_FLAGS}
)

target_link_libraries(Entity_unit_tests
    PRIVATE
        gtest
        ${EXTERNAL_LIB_DIR}/libimgui.a
)

add_test(Entity_unit_tests Entity_unit_tests)

### Pool Tests ###
add_executable(Pool_unit_tests
    ${ROOT_DIR}/test/unit/Pool_unit_tests.cpp
)

target_include_directories(Pool_unit_tests
    PRIVATE
        ${NC_INCLUDE_DIRS}
        ${ROOT_DIR}/external/gtest/googletest/include
)

target_compile_options(Pool_unit_tests
    PUBLIC
        ${GCC_WARNING_FLAGS}
)

target_link_libraries(Pool_unit_tests
    PRIVATE
        gtest
)

add_test(Pool_unit_tests Pool_unit_tests)

### PacketBuffer Tests ###
add_executable(PacketBuffer_unit_tests
    ${ROOT_DIR}/test/unit/PacketBuffer_unit_tests.cpp
    ${ROOT_DIR}/nc/source/net/PacketBuffer.cpp
)

target_include_directories(PacketBuffer_unit_tests
    PRIVATE
        ${NC_INCLUDE_DIRS}
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