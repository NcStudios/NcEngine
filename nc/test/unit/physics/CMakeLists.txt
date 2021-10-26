### UnionFind Tests ###
add_executable(IslandBuilder_unit_tests
    ${ROOT_DIR}/test/unit/physics/IslandBuilder_unit_tests.cpp
    ${ROOT_DIR}/source/physics/IslandBuilder.cpp
)

target_include_directories(IslandBuilder_unit_tests
    PRIVATE
        ${NC_INCLUDE_DIR}
        ${NC_SOURCE_DIR}
        ${EXTERNAL_INCLUDE_DIR}
        ${ROOT_DIR}/external/gtest/googletest/include
)

target_compile_options(IslandBuilder_unit_tests
    PUBLIC
        ${GCC_WARNING_FLAGS}
)

target_link_libraries(IslandBuilder_unit_tests
    PRIVATE
        gtest
)

add_test(IslandBuilder_unit_tests IslandBuilder_unit_tests)