cmake_minimum_required(VERSION 3.20)
project("NcEngineTests" LANGUAGES CXX C)

if(${PROJECT_SOURCE_DIR} EQUAL ${PROJECT_BINARY_DIR})
    message(FATAL_ERROR "Don't be a fool, out-of-source build with your tool.")
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(NC_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/../sdk/engine/include")
set(NC_SOURCE_DIR "${PROJECT_SOURCE_DIR}/../sdk/engine/source")
set(NC_EXTERNAL_DIR "${PROJECT_SOURCE_DIR}/../sdk/external")

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set(WARNING_FLAGS -Werror -Wall -Wextra -Wfatal-errors -Wno-cast-function-type)
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    set(WARNING_FLAGS /W3 /WX)
endif()

enable_testing()

include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/NcStudios/googletest.git
  GIT_TAG        97a467571a0f615a4d96e79e4399c43221ca1232
)

FetchContent_MakeAvailable(googletest)

include(GoogleTest)

add_definitions(-DNC_ASSERT_ENABLED)

add_subdirectory(unit)