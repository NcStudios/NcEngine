### FreeComponentGroup Tests ###
add_executable(FreeComponentGroup_unit_tests
    FreeComponentGroup_unit_tests.cpp
    ${NC_SOURCE_DIR}/ecs/FreeComponentGroup.cpp
)

target_include_directories(FreeComponentGroup_unit_tests
    PRIVATE
        ${NC_INCLUDE_DIR}
        ${NC_SOURCE_DIR}
        ${NC_EXTERNAL_DIR}/gtest/googletest/include
)

target_compile_options(FreeComponentGroup_unit_tests
    PUBLIC
        ${WARNING_FLAGS}
)

target_link_libraries(FreeComponentGroup_unit_tests
    PRIVATE
        gtest
)

add_test(FreeComponentGroup_unit_tests FreeComponentGroup_unit_tests)

### Entity Tests ###
add_executable(Entity_unit_tests
    Entity_unit_tests.cpp
)

target_include_directories(Entity_unit_tests
    PRIVATE
        ${NC_INCLUDE_DIR}
        ${NC_EXTERNAL_DIR}/gtest/googletest/include
)

target_compile_options(Entity_unit_tests
    PUBLIC
        ${WARNING_FLAGS}
)

target_link_libraries(Entity_unit_tests
    PRIVATE
        gtest
)

add_test(Entity_unit_tests Entity_unit_tests)

### Registry Tests ###
add_executable(Registry_unit_tests
    Registry_unit_tests.cpp
    ${NC_SOURCE_DIR}/ecs/Registry.cpp
)

target_include_directories(Registry_unit_tests
    PRIVATE
        ${NC_INCLUDE_DIR}
        ${NC_SOURCE_DIR}
        ${NC_EXTERNAL_DIR}
        ${NC_EXTERNAL_DIR}/gtest/googletest/include
)

target_compile_options(Registry_unit_tests
    PUBLIC
        ${WARNING_FLAGS}
)

target_link_libraries(Registry_unit_tests
    PRIVATE
        gtest
)

add_test(Registry_unit_tests Registry_unit_tests)

### Transform Tests ###
add_executable(Transform_unit_tests
    Transform_unit_tests.cpp
    ${NC_SOURCE_DIR}/ecs/Transform.cpp
    ${NC_SOURCE_DIR}/ecs/Registry.cpp
    ${NC_SOURCE_DIR}/math/Quaternion.cpp
)

target_include_directories(Transform_unit_tests
    PRIVATE
        ${NC_INCLUDE_DIR}
        ${NC_SOURCE_DIR}
        ${NC_EXTERNAL_DIR}
        ${NC_EXTERNAL_DIR}/gtest/googletest/include
)

target_compile_options(Transform_unit_tests
    PUBLIC
        ${WARNING_FLAGS}
)

target_link_libraries(Transform_unit_tests
    PRIVATE
        gtest
        DirectXMath
)

add_test(Transform_unit_tests Transform_unit_tests)

### View Tests ###
add_executable(View_unit_tests
    View_unit_tests.cpp
    ${NC_SOURCE_DIR}/ecs/Registry.cpp
)

target_include_directories(View_unit_tests
    PRIVATE
        ${NC_INCLUDE_DIR}
        ${NC_SOURCE_DIR}
        ${NC_EXTERNAL_DIR}
        ${NC_EXTERNAL_DIR}/gtest/googletest/include
)

target_compile_options(View_unit_tests
    PUBLIC
        ${WARNING_FLAGS}
)

target_link_libraries(View_unit_tests
    PRIVATE
        gtest
)

add_test(View_unit_tests View_unit_tests)