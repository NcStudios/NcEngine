# Gets added in debug build; removing so tests don't have to link against imgui
remove_definitions(-DNC_EDITOR_ENABLED)

### Allocator Tests ###
add_executable(Allocator_unit_tests
    ${ROOT_DIR}/test/unit/Allocator_unit_tests.cpp
)

target_include_directories(Allocator_unit_tests
    PRIVATE
        ${NC_INCLUDE_DIR}
        ${NC_SOURCE_DIR}
        ${ROOT_DIR}/external/gtest/googletest/include
)

target_compile_options(Allocator_unit_tests
    PUBLIC
        ${GCC_WARNING_FLAGS}
)

target_link_libraries(Allocator_unit_tests
    PRIVATE
        gtest
)

add_test(Allocator_unit_tests Allocator_unit_tests)

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

### EntityHandle Tests ###
add_executable(EntityHandle_unit_tests
    ${ROOT_DIR}/test/unit/EntityHandle_unit_tests.cpp
)

target_include_directories(EntityHandle_unit_tests
    PRIVATE
        ${NC_INCLUDE_DIR}
        ${EXTERNAL_INCLUDE_DIR}
        ${ROOT_DIR}/external/gtest/googletest/include
)

target_compile_options(EntityHandle_unit_tests
    PUBLIC
        ${GCC_WARNING_FLAGS}
)

target_link_libraries(EntityHandle_unit_tests
    PRIVATE
        gtest
)

add_test(EntityHandle_unit_tests EntityHandle_unit_tests)

### MakeUnique Tests ###
add_executable(MakeUnique_unit_tests
    ${ROOT_DIR}/test/unit/MakeUnique_unit_tests.cpp
)

target_include_directories(MakeUnique_unit_tests
    PRIVATE
        ${NC_INCLUDE_DIR}
        ${NC_SOURCE_DIR}
        ${ROOT_DIR}/external/gtest/googletest/include
)

target_compile_options(MakeUnique_unit_tests
    PUBLIC
        ${GCC_WARNING_FLAGS}
)

target_link_libraries(MakeUnique_unit_tests
    PRIVATE
        gtest
)

add_test(MakeUnique_unit_tests MakeUnique_unit_tests)

### MemoryResource Tests ###
add_executable(MemoryResource_unit_tests
    ${ROOT_DIR}/test/unit/MemoryResource_unit_tests.cpp
)

target_include_directories(MemoryResource_unit_tests
    PRIVATE
        ${NC_INCLUDE_DIR}
        ${NC_SOURCE_DIR}
        ${ROOT_DIR}/external/gtest/googletest/include
)

target_compile_options(MemoryResource_unit_tests
    PUBLIC
        ${GCC_WARNING_FLAGS}
)

target_link_libraries(MemoryResource_unit_tests
    PRIVATE
        gtest
)

add_test(MemoryResource_unit_tests MemoryResource_unit_tests)

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

### Registry Tests ###
add_executable(Registry_unit_tests
    ${ROOT_DIR}/test/unit/Registry_unit_tests.cpp
)

target_include_directories(Registry_unit_tests
    PRIVATE
        ${NC_INCLUDE_DIR}
        ${NC_SOURCE_DIR}
        ${ROOT_DIR}/external/gtest/googletest/include
        #delete
        ${EXTERNAL_INCLUDE_DIR}
)

target_compile_options(Registry_unit_tests
    PUBLIC
        ${GCC_WARNING_FLAGS}
)

target_link_libraries(Registry_unit_tests
    PRIVATE
        gtest
)

add_test(Registry_unit_tests Registry_unit_tests)

### Transform Tests ###
add_executable(Transform_unit_tests
    ${ROOT_DIR}/test/unit/Transform_unit_tests.cpp
    #${ROOT_DIR}/source/component/Component.cpp
    #${ROOT_DIR}/source/component/Transform.cpp
    #${ROOT_DIR}/source/math/Quaternion.cpp
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