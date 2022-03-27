cmake_minimum_required(VERSION 3.20)
project("NcEngineTests" LANGUAGES CXX C)

if(${PROJECT_SOURCE_DIR} EQUAL ${PROJECT_BINARY_DIR})
    message(FATAL_ERROR "Don't be a fool, out-of-source build with your tool.")
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(NC_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/../engine/include")
set(NC_SOURCE_DIR "${PROJECT_SOURCE_DIR}/../engine/source")
set(NC_EXTERNAL_DIR "${PROJECT_SOURCE_DIR}/../engine/external")

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set(WARNING_FLAGS -Werror -Wall -Wextra -Wfatal-errors -Wno-cast-function-type)
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    set(WARNING_FLAGS /W3 /WX)
endif()

enable_testing()
add_subdirectory(${NC_EXTERNAL_DIR}/gtest ${PROJECT_BINARY_DIR}/gtest)
include(GoogleTest)

add_definitions(-DNC_ASSERT_ENABLED)

add_subdirectory(unit)