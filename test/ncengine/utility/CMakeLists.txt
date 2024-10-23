### Log Tests ###
add_executable(Log_tests
    Log_tests.cpp
    ${NC_SOURCE_DIR}/utility/Log.cpp
    ${NC_SOURCE_DIR}/utility/FileLogger.cpp
)

target_include_directories(Log_tests
    PRIVATE
        ${NC_INCLUDE_DIR}
)

target_compile_definitions(Log_tests
    PRIVATE
        -DNC_LOG_LEVEL=2
)

target_compile_options(Log_tests
    PUBLIC
        ${NC_COMPILER_FLAGS}
)

target_link_libraries(Log_tests
    PRIVATE
        NcUtility
        gtest_main
)

add_test(Log_tests Log_tests)

### Signal Tests ###
add_executable(Signal_unit_tests
    Signal_unit_tests.cpp
)

target_include_directories(Signal_unit_tests
    PRIVATE
        ${NC_INCLUDE_DIR}
        ${NC_INCLUDE_DIR}/ncengine
)

target_compile_options(Signal_unit_tests
    PUBLIC
        ${NC_COMPILER_FLAGS}
)

target_link_libraries(Signal_unit_tests
    PRIVATE
        NcUtility
        gtest_main
)

add_test(Signal_unit_tests Signal_unit_tests)

### SparseMap Tests ###
add_executable(SparseMap_unit_tests
SparseMap_unit_tests.cpp
)

target_include_directories(SparseMap_unit_tests
    PRIVATE
        ${NC_INCLUDE_DIR}
)

target_compile_options(SparseMap_unit_tests
    PUBLIC
        ${NC_COMPILER_FLAGS}
)

target_link_libraries(SparseMap_unit_tests
    PRIVATE
        NcUtility
        gtest_main
)

add_test(SparseMap_unit_tests SparseMap_unit_tests)
