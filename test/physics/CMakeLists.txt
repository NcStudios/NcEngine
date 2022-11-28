### Manifold Tests ###
add_executable(Manifold_unit_tests
    Manifold_unit_tests.cpp
    ${NC_SOURCE_DIR}/physics/PhysicsPipelineTypes.cpp
)

target_include_directories(Manifold_unit_tests
    PRIVATE
        ${NC_INCLUDE_DIR}
        ${NC_INCLUDE_DIR}/ncengine
        ${NC_SOURCE_DIR}
        ${NC_EXTERNAL_DIR}
)

target_compile_options(Manifold_unit_tests
    PUBLIC
        ${NC_COMPILER_FLAGS}
)

target_link_libraries(Manifold_unit_tests
    PRIVATE
        NcMath
        gtest
)

add_test(Manifold_unit_tests Manifold_unit_tests)

### ClientObjectProperties Tests ###
add_executable(ClientObjectProperties_unit_tests
    ClientObjectProperties_unit_tests.cpp
    ${NC_SOURCE_DIR}/physics/PhysicsPipelineTypes.cpp
)

target_include_directories(ClientObjectProperties_unit_tests
    PRIVATE
        ${NC_INCLUDE_DIR}
        ${NC_INCLUDE_DIR}/ncengine
        ${NC_SOURCE_DIR}
        ${NC_EXTERNAL_DIR}
)

target_compile_options(ClientObjectProperties_unit_tests
    PUBLIC
        ${NC_COMPILER_FLAGS}
)

target_link_libraries(ClientObjectProperties_unit_tests
    PRIVATE
        NcMath
        gtest
)

add_test(ClientObjectProperties_unit_tests ClientObjectProperties_unit_tests)

add_subdirectory(collision)