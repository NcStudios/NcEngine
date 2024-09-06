### Shape Tests ###
add_executable(Contacts_unit_tests
    Contacts_unit_tests.cpp
)

target_include_directories(Contacts_unit_tests
    PRIVATE
        ${NC_INCLUDE_DIR}
)

target_compile_options(Contacts_unit_tests
    PUBLIC
        ${NC_COMPILER_FLAGS}
)

target_link_libraries(Contacts_unit_tests
    PRIVATE
        NcMath
        NcUtility
        gtest_main
)

add_test(Contacts_unit_tests Contacts_unit_tests)

### PhysicsUtility Tests ###
add_executable(PhysicsUtility_unit_tests
    PhysicsUtility_unit_tests.cpp
    ${NC_SOURCE_DIR}/physics2/PhysicsUtility.cpp
)

target_include_directories(PhysicsUtility_unit_tests
    PRIVATE
        ${NC_INCLUDE_DIR}
)

target_compile_options(PhysicsUtility_unit_tests
    PUBLIC
        ${NC_COMPILER_FLAGS}
)

target_link_libraries(PhysicsUtility_unit_tests
    PRIVATE
        NcMath
        NcUtility
        gtest_main
)

add_test(PhysicsUtility_unit_tests PhysicsUtility_unit_tests)

### Shape Tests ###
add_executable(Shape_unit_tests
    Shape_unit_tests.cpp
    ${NC_SOURCE_DIR}/physics2/Shape.cpp
)

target_include_directories(Shape_unit_tests
    PRIVATE
        ${NC_INCLUDE_DIR}
        ${NC_SOURCE_DIR}
)

target_compile_options(Shape_unit_tests
    PUBLIC
        ${NC_COMPILER_FLAGS}
)

target_link_libraries(Shape_unit_tests
    PRIVATE
        NcMath
        NcUtility
        gtest_main
)

add_test(Shape_unit_tests Shape_unit_tests)

add_subdirectory(jolt)
