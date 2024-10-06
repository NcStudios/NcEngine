### NcScene Tests ###
add_executable(NcScene_unit_tests
    NcScene_unit_tests.cpp
    ${NC_SOURCE_DIR}/scene/NcSceneImpl.cpp
)

target_include_directories(NcScene_unit_tests
    PRIVATE
        ${NC_INCLUDE_DIR}
        ${NC_INCLUDE_DIR}/ncengine
        ${NC_SOURCE_DIR}
        ${NC_EXTERNAL_DIR}
)

target_compile_options(NcScene_unit_tests
    PUBLIC
        ${NC_COMPILER_FLAGS}
)

target_link_libraries(NcScene_unit_tests
    PRIVATE
        gtest_main
        NcMath
        NcUtility
)

add_test(NcScene_unit_tests NcScene_unit_tests)
