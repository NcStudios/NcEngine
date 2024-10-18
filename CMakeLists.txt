cmake_minimum_required(VERSION 3.20)
project("NcSdk"
        VERSION   "0.1.0"
        LANGUAGES CXX C
)

# Options
option(NC_BUILD_TESTS "Enable building tests." OFF)
option(NC_BUILD_INTEGRATION_TESTS "Enable building integration tests." OFF)
option(NC_BUILD_NCCONVERT "Enable building nc-convert asset converter." ON)
option(NC_RUNTIME_SHADER_COMPILATION "Enable compiling shaders at runtime" ON)
option(NC_PROFILING_ENABLED "Enable profiling with Optick" OFF)
option(NC_PROD_BUILD "Only build NcEngine production binaries." OFF)

# Diligent feature flag
option(NC_USE_DILIGENT "Enable DiligentGraphics" OFF)
if(NC_USE_DILIGENT)
    add_definitions(-DNC_USE_DILIGENT)
endif()

# Variables
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set(NC_COMPILER_FLAGS -Werror
                          -Wfatal-errors
                          -Wall
                          -Wextra
                          -Wpedantic
                          -Wshadow
                          -Wconversion
                          -Wno-cast-function-type
                          -Warray-bounds=0 # would like to have this, but too many false positives right now
    )
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    set(NC_COMPILER_FLAGS /W4
                          /WX
                          /MP
                          /arch:AVX2
    )

    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()
set(NC_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/include")
set(NC_SOURCE_DIR "${PROJECT_SOURCE_DIR}/source/ncengine/")
set(NC_EXTERNAL_DIR "${PROJECT_SOURCE_DIR}/source/external")
if(NC_PROD_BUILD)
    set(NC_COMPILE_DEFINITIONS NC_LOG_LEVEL=1)
    set(NC_ENGINE_LIB "NcEngine")
    if (NOT DEFINED NC_BUILD_NCCONVERT)
        set(NC_BUILD_NCCONVERT OFF CACHE BOOL "" FORCE)
    endif()
    if(NOT DEFINED NC_RUNTIME_SHADER_COMPILATION)
        set(NC_RUNTIME_SHADER_COMPILATION OFF CACHE BOOL "" FORCE)
    endif()
else()
    set(NC_COMPILE_DEFINITIONS NC_LOG_LEVEL=2 NC_EDITOR_ENABLED NC_DEBUG_RENDERING_ENABLED NC_ASSERT_ENABLED)
    set(NC_ENGINE_LIB "NcEngine-dev")
    set(NC_SAMPLE_EXE "Sample")
endif()

if(NC_RUNTIME_SHADER_COMPILATION)
    list(APPEND NC_COMPILE_DEFINITIONS NC_RUNTIME_SHADER_COMPILATION)
endif()

if(NC_PROFILING_ENABLED)
    list(APPEND NC_COMPILE_DEFINITIONS NC_PROFILING_ENABLED)
endif()

# TODO: #494 Remove usage of aligned_storage
add_definitions(-D_SILENCE_CXX23_ALIGNED_STORAGE_DEPRECATION_WARNING)

# Get dependencies
include(cmake/FetchDependencies.cmake)

# Include runtime dependency helpers.
include(cmake/InstallDependencies.cmake)

# Add source
add_subdirectory(source)

if(NOT NC_PROD_BUILD)
    add_subdirectory(sample)
endif()

if(NC_BUILD_TESTS)
    add_subdirectory(test)
endif()

# Install Default Assets
install(DIRECTORY      ${PROJECT_SOURCE_DIR}/resources/
        DESTINATION    resources/assets
        FILES_MATCHING REGEX ".*\.(nca)|(spv)"
)

# Install External Files
install(DIRECTORY   ${PROJECT_SOURCE_DIR}/source/external/imgui
        DESTINATION include
        PATTERN     "src" EXCLUDE
)
