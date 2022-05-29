# Fetch gtest
set(BUILD_GMOCK OFF)
set(INSTALL_GTEST OFF)

FetchContent_Declare(googletest
                     GIT_REPOSITORY https://github.com/NcStudios/googletest.git
                     GIT_TAG        origin/main
)

FetchContent_MakeAvailable(googletest)

include(GoogleTest)

enable_testing()

add_definitions(-DNC_ASSERT_ENABLED)

add_subdirectory(unit)