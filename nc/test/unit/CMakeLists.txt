# Gets added in debug build; removing so tests don't have to link against imgui
remove_definitions(-DNC_EDITOR_ENABLED)

### ComponentSystem Tests ###
add_executable(ComponentSystem_unit_tests
    ${ROOT_DIR}/test/unit/ComponentSystem_unit_tests.cpp
    ${ROOT_DIR}/source/component/Component.cpp
)

target_include_directories(ComponentSystem_unit_tests
    PRIVATE
        ${NC_INCLUDE_DIR}
        ${NC_SOURCE_DIR}
        ${EXTERNAL_INCLUDE_DIR}
        ${ROOT_DIR}/external/gtest/googletest/include
)

target_compile_options(ComponentSystem_unit_tests
    PUBLIC
        ${GCC_WARNING_FLAGS}
)

target_link_libraries(ComponentSystem_unit_tests
    PRIVATE
        gtest
)

add_test(ComponentSystem_unit_tests ComponentSystem_unit_tests)

### Entity Tests ###
add_executable(Entity_unit_tests
    ${ROOT_DIR}/test/unit/Entity_unit_tests.cpp
    ${ROOT_DIR}/source/component/Component.cpp
    ${ROOT_DIR}/source/ecs/Entity.cpp
)

target_include_directories(Entity_unit_tests
    PRIVATE
        ${NC_INCLUDE_DIR}
        ${NC_SOURCE_DIR}
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
)

add_test(Entity_unit_tests Entity_unit_tests)

### Pool Tests ###
add_executable(Pool_unit_tests
    ${ROOT_DIR}/test/unit/Pool_unit_tests.cpp
)

target_include_directories(Pool_unit_tests
    PRIVATE
        ${NC_INCLUDE_DIR}
        ${NC_SOURCE_DIR}
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

### Transform Tests ###
add_executable(Transform_unit_tests
    ${ROOT_DIR}/test/unit/Transform_unit_tests.cpp
    ${ROOT_DIR}/source/component/Component.cpp
    ${ROOT_DIR}/source/component/Transform.cpp
    ${ROOT_DIR}/source/math/Quaternion.cpp
    ${ROOT_DIR}/source/math/Vector3.cpp
)

target_include_directories(Transform_unit_tests
    PRIVATE
        ${NC_INCLUDE_DIR}
        ${NC_SOURCE_DIR}
        ${EXTERNAL_INCLUDE_DIR}
        ${ROOT_DIR}/external/gtest/googletest/include
)

target_compile_options(Transform_unit_tests
    PUBLIC
        ${GCC_WARNING_FLAGS}
)

target_link_libraries(Transform_unit_tests
    PRIVATE
        gtest
)

add_test(Transform_unit_tests Transform_unit_tests)