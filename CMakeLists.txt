cmake_minimum_required(VERSION 3.20)
project("NcSdk"
        VERSION   "0.1.0"
        LANGUAGES CXX C
)

# Options
option(NC_BUILD_TESTS "Enable building tests." OFF)
option(NC_PROFILING_ENABLED "Enable profiling with Optick" OFF)
option(NC_PROD_BUILD "Only build NcEngine production binaries." OFF)

# Variables
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_DEBUG_POSTFIX d)
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
endif()
set(NC_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/include")
set(NC_SOURCE_DIR "${PROJECT_SOURCE_DIR}/source/engine/")
set(NC_EXTERNAL_DIR "${PROJECT_SOURCE_DIR}/source/external")
if(NC_PROD_BUILD)
    set(NC_COMPILE_DEFINITIONS NC_LOG_LEVEL=1)
    set(NC_ENGINE_LIB "NcEngine")
else()
    set(NC_COMPILE_DEFINITIONS NC_LOG_LEVEL=2 NC_EDITOR_ENABLED NC_DEBUG_RENDERING_ENABLED NC_ASSERT_ENABLED)
    set(NC_ENGINE_LIB "NcEngine-dev")
    set(NC_EDITOR_EXE "NcEditor")
    set(NC_SAMPLE_EXE "Sample")
endif()

# TODO: #494 Remove usage of aligned_storage
add_definitions(-D_SILENCE_CXX23_ALIGNED_STORAGE_DEPRECATION_WARNING)

# Get dependencies
include(cmake/FetchDependencies.cmake)

# Add source
add_subdirectory(source)

add_subdirectory(DiligentCore)

if(NOT NC_PROD_BUILD)
    add_subdirectory(sample)
endif()

if(NC_BUILD_TESTS)
    add_subdirectory(test)
endif()

# Install nc-convert
if(UNIX AND NOT APPLE)
    install(FILES       ${nc-convert_SOURCE_DIR}/nc-convert
            DESTINATION bin
    )
elseif(WIN32)
    install(FILES       ${nc-convert_SOURCE_DIR}/nc-convert.exe
            DESTINATION bin
    )
elseif(APPLE)
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
