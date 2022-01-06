cmake_minimum_required(VERSION 3.20)
project("NcEngineTests" LANGUAGES CXX C)

if(${PROJECT_SOURCE_DIR} EQUAL ${PROJECT_BINARY_DIR})
    message(FATAL_ERROR "Don't be a fool, out-of-source build with your tool.")
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(NC_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/../nc/include")
set(NC_SOURCE_DIR "${PROJECT_SOURCE_DIR}/../nc/source")
set(NC_EXTERNAL_DIR "${PROJECT_SOURCE_DIR}/../nc/external")

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set(WARNING_FLAGS -Wall -Wextra -Wfatal-errors -Wno-cast-function-type)
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
endif()

enable_testing()
add_subdirectory(${NC_EXTERNAL_DIR}/gtest ${PROJECT_BINARY_DIR}/gtest)
include(GoogleTest)

add_definitions(-DNC_DEBUG_BUILD)

add_subdirectory(unit)