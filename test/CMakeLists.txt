# Fetch gtest
set(BUILD_GMOCK OFF CACHE BOOL "" FORCE)
set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)

enable_testing()

FetchContent_Declare(googletest
                     GIT_REPOSITORY https://github.com/NcStudios/googletest.git
                     GIT_TAG        origin/main
)

FetchContent_MakeAvailable(googletest)

include(GoogleTest)

add_definitions(-DNC_ASSERT_ENABLED)

add_subdirectory(alloc)
add_subdirectory(assets)
add_subdirectory(config)
add_subdirectory(ecs)
add_subdirectory(math)
add_subdirectory(module)
add_subdirectory(network)
add_subdirectory(physics)
add_subdirectory(serialize)
add_subdirectory(utility)
