### PerFrameProxyCache Tests ###
add_executable(PerFrameProxyCache_unit_tests
    ${PROJECT_SOURCE_DIR}/unit/physics/proxy/PerFrameProxyCache_unit_tests.cpp
    ${NC_SOURCE_DIR}/physics/PhysicsPipelineTypes.cpp
    ${NC_SOURCE_DIR}/math/Quaternion.cpp
)

target_include_directories(PerFrameProxyCache_unit_tests
    PRIVATE
        ${NC_INCLUDE_DIR}
        ${NC_SOURCE_DIR}
        ${NC_EXTERNAL_DIR}
        ${NC_EXTERNAL_DIR}/gtest/googletest/include
)

target_compile_options(PerFrameProxyCache_unit_tests
    PUBLIC
        ${WARNING_FLAGS}
)

target_link_libraries(PerFrameProxyCache_unit_tests
    PRIVATE
        gtest
)

add_test(PerFrameProxyCache_unit_tests PerFrameProxyCache_unit_tests)