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

# TODO: RtAudio uses deprecated wchar_t conversion
add_definitions(-D_SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING)

# Get dependencies
find_package(Vulkan REQUIRED)

include(FetchContent)

FetchContent_Declare(NcCommon
                     GIT_REPOSITORY https://github.com/NcStudios/NcCommon.git
                     GIT_TAG        origin/release
                     GIT_SHALLOW    TRUE
)

set(NC_TOOLS_BUILD_CONVERTER OFF)
set(NC_TOOLS_BUILD_TESTS OFF)
FetchContent_Declare(nc-tools
                     GIT_REPOSITORY      https://github.com/NcStudios/NcTools.git
                     GIT_TAG             v1.0.0
                     GIT_SHALLOW         TRUE
)

# Get pre-built nc-convert.exe from NcTools so we don't have to build it and its dependencies
FetchContent_Declare(nc-convert
                     URL                 https://github.com/NcStudios/NcTools/releases/download/v1.0.0/nc-convert.exe
                     URL_HASH            SHA256=281FA6A37D6A72111EF8586F4A8A415F3C9D52CA44B956E143911813A5759A3F
                     DOWNLOAD_NO_EXTRACT TRUE
)

FetchContent_Declare(taskflow
                     GIT_REPOSITORY https://github.com/NcStudios/taskflow.git
                     GIT_TAG        ab6c1b6cd2651d4a4b1a3204b2e1b95f75d06a56 # origin/master
                     GIT_SHALLOW    TRUE
)

FetchContent_Declare(glfw
                     GIT_REPOSITORY https://github.com/NcStudios/glfw.git
                     GIT_TAG        8f470597d625ae28758c16b4293dd42d63e8a83a # origin/master
                     GIT_SHALLOW    TRUE
)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(NcCommon nc-tools nc-convert taskflow glfw)

# Add source
add_subdirectory(source)

if(NOT NC_PROD_BUILD)
    add_subdirectory(sample)
endif()

if(NC_BUILD_TESTS)
    add_subdirectory(test)
endif()

# Install nc-convert executable
install(FILES       ${nc-convert_SOURCE_DIR}/nc-convert.exe
        DESTINATION bin
)

# Install Default Assets
install(DIRECTORY      ${PROJECT_SOURCE_DIR}/resources/
        DESTINATION    resources/assets
        FILES_MATCHING REGEX ".*\.(nca)|(spv)"
)

# Install External Files
install(DIRECTORY   ${PROJECT_SOURCE_DIR}/source/external/enet
                    ${PROJECT_SOURCE_DIR}/source/external/imgui
        DESTINATION include
        PATTERN     "src" EXCLUDE
)
