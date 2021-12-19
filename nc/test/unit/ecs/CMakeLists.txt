### AutoComponentGroup Tests ###
add_executable(AutoComponentGroup_unit_tests
    ${ROOT_DIR}/test/unit/ecs/AutoComponentGroup_unit_tests.cpp
    ${ROOT_DIR}/source/ecs/component/AutoComponentGroup.cpp
)

target_include_directories(AutoComponentGroup_unit_tests
    PRIVATE
        ${NC_INCLUDE_DIR}
        ${NC_SOURCE_DIR}
        ${ROOT_DIR}/external/gtest/googletest/include
)

target_compile_options(AutoComponentGroup_unit_tests
    PUBLIC
        ${WARNING_FLAGS}
)

target_link_libraries(AutoComponentGroup_unit_tests
    PRIVATE
        gtest
)

add_test(AutoComponentGroup_unit_tests AutoComponentGroup_unit_tests)

### Entity Tests ###
add_executable(Entity_unit_tests
    ${ROOT_DIR}/test/unit/ecs/Entity_unit_tests.cpp
)

target_include_directories(Entity_unit_tests
    PRIVATE
        ${NC_INCLUDE_DIR}
        ${ROOT_DIR}/external/gtest/googletest/include
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
    ${ROOT_DIR}/test/unit/ecs/Registry_unit_tests.cpp
    ${ROOT_DIR}/source/ecs/Registry.cpp
)

target_include_directories(Registry_unit_tests
    PRIVATE
        ${NC_INCLUDE_DIR}
        ${NC_SOURCE_DIR}
        ${NC_EXTERNAL_DIR}
        ${ROOT_DIR}/external/gtest/googletest/include
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
    ${ROOT_DIR}/test/unit/ecs/Transform_unit_tests.cpp
    ${ROOT_DIR}/source/ecs/component/Transform.cpp
    ${ROOT_DIR}/source/ecs/Registry.cpp
    ${ROOT_DIR}/source/math/Quaternion.cpp
)

target_include_directories(Transform_unit_tests
    PRIVATE
        ${NC_INCLUDE_DIR}
        ${NC_SOURCE_DIR}
        ${NC_EXTERNAL_DIR}
        ${ROOT_DIR}/external/gtest/googletest/include
)

target_compile_options(Transform_unit_tests
    PUBLIC
        ${WARNING_FLAGS}
)

target_link_libraries(Transform_unit_tests
    PRIVATE
        gtest
)

add_test(Transform_unit_tests Transform_unit_tests)