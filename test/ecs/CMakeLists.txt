### AccessPolicy Tests ###
add_executable(AccessPolicy_unit_tests
    AccessPolicy_unit_tests.cpp
)

target_include_directories(AccessPolicy_unit_tests
    PRIVATE
        ${NC_INCLUDE_DIR}
        ${NC_EXTERNAL_DIR}
)

target_compile_options(AccessPolicy_unit_tests
    PUBLIC
        ${NC_COMPILER_FLAGS}
)

target_link_libraries(AccessPolicy_unit_tests
    PRIVATE
        NcMath
        NcUtility
        gtest_main
)

add_test(AccessPolicy_unit_tests AccessPolicy_unit_tests)

### AnyComponent Tests ###
add_executable(AnyComponent_unit_test
    AnyComponent_unit_tests.cpp
)

target_include_directories(AnyComponent_unit_test
    PRIVATE
        ${NC_INCLUDE_DIR}
)

target_compile_options(AnyComponent_unit_test
    PUBLIC
        ${NC_COMPILER_FLAGS}
)

target_link_libraries(AnyComponent_unit_test
    PRIVATE
        NcUtility
        gtest_main
)

add_test(AnyComponent_unit_test AnyComponent_unit_test)

### ComponentPool Tests ###
add_executable(ComponentPool_unit_tests
    ComponentPool_unit_tests.cpp
)

target_include_directories(ComponentPool_unit_tests
    PRIVATE
        ${NC_INCLUDE_DIR}
        ${NC_EXTERNAL_DIR}
)

target_compile_options(ComponentPool_unit_tests
    PUBLIC
        ${NC_COMPILER_FLAGS}
)

target_link_libraries(ComponentPool_unit_tests
    PRIVATE
        NcMath
        NcUtility
        gtest_main
)

add_test(ComponentPool_unit_tests ComponentPool_unit_tests)

### ComponentRegistry Tests ###
add_executable(ComponentRegistry_unit_tests
    ComponentRegistry_unit_tests.cpp
)

target_include_directories(ComponentRegistry_unit_tests
    PRIVATE
        ${NC_INCLUDE_DIR}
        ${NC_EXTERNAL_DIR}
)

target_compile_options(ComponentRegistry_unit_tests
    PUBLIC
        ${NC_COMPILER_FLAGS}
)

target_link_libraries(ComponentRegistry_unit_tests
    PRIVATE
        NcMath
        NcUtility
        gtest_main
)

add_test(ComponentRegistry_unit_tests ComponentRegistry_unit_tests)

### EcsInterface Tests ###
add_executable(EcsInterface_unit_tests
    EcsInterface_unit_tests.cpp
    ${NC_SOURCE_DIR}/ecs/FreeComponentGroup.cpp
    ${NC_SOURCE_DIR}/ecs/Transform.cpp
)

target_include_directories(EcsInterface_unit_tests
    PRIVATE
        ${NC_INCLUDE_DIR}
        ${NC_EXTERNAL_DIR}
)

target_compile_options(EcsInterface_unit_tests
    PUBLIC
        ${NC_COMPILER_FLAGS}
)

target_link_libraries(EcsInterface_unit_tests
    PRIVATE
        NcMath
        NcUtility
        gtest_main
)

add_test(EcsInterface_unit_tests EcsInterface_unit_tests)

### Entity Tests ###
add_executable(Entity_unit_tests
    Entity_unit_tests.cpp
)

target_include_directories(Entity_unit_tests
    PRIVATE
        ${NC_INCLUDE_DIR}
        ${NC_INCLUDE_DIR}/ncengine
)

target_compile_options(Entity_unit_tests
    PUBLIC
        ${NC_COMPILER_FLAGS}
)

target_link_libraries(Entity_unit_tests
    PRIVATE
        NcMath
        gtest_main
)

add_test(Entity_unit_tests Entity_unit_tests)

### EntityPool Tests ###
add_executable(EntityPool_unit_tests
    EntityPool_unit_tests.cpp
)

target_include_directories(EntityPool_unit_tests
    PRIVATE
        ${NC_INCLUDE_DIR}
        ${NC_EXTERNAL_DIR}
)

target_compile_options(EntityPool_unit_tests
    PUBLIC
        ${NC_COMPILER_FLAGS}
)

target_link_libraries(EntityPool_unit_tests
    PRIVATE
        NcMath
        NcUtility
        gtest_main
)

add_test(EntityPool_unit_tests EntityPool_unit_tests)

### FreeComponentGroup Tests ###
add_executable(FreeComponentGroup_unit_test
    FreeComponentGroup_unit_tests.cpp
    ${NC_SOURCE_DIR}/ecs/FreeComponentGroup.cpp
)

target_include_directories(FreeComponentGroup_unit_test
    PRIVATE
        ${NC_INCLUDE_DIR}
        ${NC_INCLUDE_DIR}/ncengine
        ${NC_SOURCE_DIR}
)

target_compile_options(FreeComponentGroup_unit_test
    PUBLIC
        ${NC_COMPILER_FLAGS}
)

target_link_libraries(FreeComponentGroup_unit_test
    PRIVATE
        NcUtility
        gtest
)

add_test(FreeComponentGroup_unit_test FreeComponentGroup_unit_test)

### Registry Tests ###
add_executable(Registry_unit_tests
    Registry_unit_tests.cpp
)

target_include_directories(Registry_unit_tests
    PRIVATE
        ${NC_INCLUDE_DIR}
        ${NC_INCLUDE_DIR}/ncengine
        ${NC_SOURCE_DIR}
        ${NC_EXTERNAL_DIR}
)

target_compile_options(Registry_unit_tests
    PUBLIC
        ${NC_COMPILER_FLAGS}
)

target_link_libraries(Registry_unit_tests
    PRIVATE
        NcMath
        NcUtility
        gtest_main
)

add_test(Registry_unit_tests Registry_unit_tests)

### SparseSet Tests ###
add_executable(SparseSet_unit_tests
    SparseSet_unit_tests.cpp
)

target_include_directories(SparseSet_unit_tests
    PRIVATE
        ${NC_INCLUDE_DIR}
        ${NC_EXTERNAL_DIR}
)

target_compile_options(SparseSet_unit_tests
    PUBLIC
        ${NC_COMPILER_FLAGS}
)

target_link_libraries(SparseSet_unit_tests
    PRIVATE
        NcMath
        NcUtility
        gtest_main
)

add_test(SparseSet_unit_tests SparseSet_unit_tests)

### Transform Tests ###
add_executable(Transform_unit_tests
    Transform_unit_tests.cpp
    ${NC_SOURCE_DIR}/ecs/Transform.cpp
)

target_include_directories(Transform_unit_tests
    PRIVATE
        ${NC_INCLUDE_DIR}
        ${NC_INCLUDE_DIR}/ncengine
        ${NC_SOURCE_DIR}
        ${NC_EXTERNAL_DIR}
)

target_compile_options(Transform_unit_tests
    PUBLIC
        ${NC_COMPILER_FLAGS}
)

target_link_libraries(Transform_unit_tests
    PRIVATE
        NcMath
        NcUtility
        gtest
)

add_test(Transform_unit_tests Transform_unit_tests)

### View Tests ###
add_executable(View_unit_tests
    View_unit_tests.cpp
)

target_include_directories(View_unit_tests
    PRIVATE
        ${NC_INCLUDE_DIR}
        ${NC_INCLUDE_DIR}/ncengine
        ${NC_SOURCE_DIR}
        ${NC_EXTERNAL_DIR}
)

target_compile_options(View_unit_tests
    PUBLIC
        ${NC_COMPILER_FLAGS}
)

target_link_libraries(View_unit_tests
    PRIVATE
        NcMath
        NcUtility
        gtest
)

add_test(View_unit_tests View_unit_tests)

