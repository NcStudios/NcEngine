enable_testing()
include(GoogleTest)

add_definitions(-DNC_ASSERT_ENABLED)

add_subdirectory(ncasset)
add_subdirectory(ncengine)
add_subdirectory(ncmath)
add_subdirectory(ncutility)

if(NC_BUILD_WIN32_INTEGRATION_TESTS OR NC_BUILD_LINUX_INTEGRATION_TESTS)
    add_subdirectory(integration_test)
endif()

if(NC_BUILD_NCCONVERT)
    add_subdirectory(ncconvert)
endif()
