enable_testing()
include(GoogleTest)

add_definitions(-DNC_ASSERT_ENABLED)

add_subdirectory(ncasset)
add_subdirectory(ncengine)
add_subdirectory(ncmath)
add_subdirectory(ncutility)

if(NC_BUILD_NCCONVERT)
    add_subdirectory(ncconvert)
endif()
