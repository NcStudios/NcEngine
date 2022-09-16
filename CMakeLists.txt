cmake_minimum_required(VERSION 3.20)
project("NcSdk"
        VERSION   "0.1.0"
        LANGUAGES CXX C
)

# Options
option(NC_BUILD_TESTS "Enable building tests." OFF)
option(NC_PROD_BUILD "Only build NcEngine production binaries." OFF)

# Variables
set(CMAKE_CXX_STANDARD 23) # we only actually need 20, but michaelsoft dances to their own beat
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_DEBUG_POSTFIX d)
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set(NC_COMPILER_FLAGS -Werror -Wall -Wextra -Wfatal-errors -Wno-cast-function-type)
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    set(NC_COMPILER_FLAGS /W3 /WX /MP)
endif()
set(NC_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/include")
set(NC_SOURCE_DIR "${PROJECT_SOURCE_DIR}/source/engine/")
set(NC_EXTERNAL_DIR "${PROJECT_SOURCE_DIR}/source/external")
set(IMGUI_LIB "imgui")
set(ENET_LIB "enet")
if(NC_PROD_BUILD)
    set(NC_COMPILE_DEFINITIONS NC_LOG_LEVEL=1)
    set(NC_ENGINE_LIB "NcEngine")
else()
    # @todo add back NC_PROFILING_ENABLED - we don't currently fetch Optick
    set(NC_COMPILE_DEFINITIONS NC_LOG_LEVEL=2 NC_EDITOR_ENABLED NC_DEBUG_RENDERING_ENABLED NC_ASSERT_ENABLED)
    set(NC_ENGINE_LIB "NcEngine-dev")
    set(NC_EDITOR_EXE "NcEditor")
    set(NC_SAMPLE_EXE "Sample")
endif()

# TODO: Taskflow has deprecation warnings for std::aligned_XXX with c++23. We eventually want to remove this.
add_definitions(-D_SILENCE_ALL_CXX23_DEPRECATION_WARNINGS)

# Get dependencies
find_package(Vulkan REQUIRED)

include(FetchContent)

FetchContent_Declare(DirectXMath
                     GIT_REPOSITORY https://github.com/NcStudios/DirectXMath.git
                     GIT_TAG        4533e8faeb4f9d251a87ca3f67b863ae25196d2c # origin/main
)

FetchContent_Declare(taskflow
                     GIT_REPOSITORY https://github.com/NcStudios/taskflow.git
                     GIT_TAG        ab6c1b6cd2651d4a4b1a3204b2e1b95f75d06a56 # origin/master
)

FetchContent_Declare(nc-tools
                     URL      https://github.com/NcStudios/NcTools/releases/download/v0.1.1/nc-tools.zip
                     URL_HASH SHA256=97BD697A4A3C6CF01713E984CF497DDCB1E2C8D09B0F6FC25E1FD6017F372671
)

FetchContent_MakeAvailable(DirectXMath taskflow nc-tools)

# Add source
add_subdirectory(source)

if(NOT NC_PROD_BUILD)
    add_subdirectory(sample)
endif()

if(NC_BUILD_TESTS)
    add_subdirectory(test)
endif()

# Install nc-tools
install(FILES       ${nc-tools_SOURCE_DIR}/nc-tools.exe
        DESTINATION bin
)

# Install Default Assets
install(DIRECTORY      ${PROJECT_SOURCE_DIR}/resources/
        DESTINATION    resources/assets
        FILES_MATCHING REGEX ".*\.(nca)|(spv)|(png)|(jpg)"
)

# Install External Files
install(DIRECTORY   ${PROJECT_SOURCE_DIR}/source/external/enet
                    ${PROJECT_SOURCE_DIR}/source/external/imgui
                    ${PROJECT_SOURCE_DIR}/source/external/vulkan
        DESTINATION include
        PATTERN     "src" EXCLUDE
)
