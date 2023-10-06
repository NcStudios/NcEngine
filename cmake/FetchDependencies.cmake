#############################
### Required Dependencies ###
#############################

# Vulkan
find_package(Vulkan REQUIRED)

include(FetchContent)

# NcCommon
FetchContent_Declare(NcCommon
                     GIT_REPOSITORY https://github.com/NcStudios/NcCommon.git
                     GIT_TAG        origin/vnext
                     GIT_SHALLOW    TRUE
)

# NcTools Libs
set(NC_TOOLS_BUILD_CONVERTER OFF)
set(NC_TOOLS_BUILD_TESTS OFF)

FetchContent_Declare(nc-tools
                     GIT_REPOSITORY      https://github.com/NcStudios/NcTools.git
                     GIT_TAG             v2.0.0
                     GIT_SHALLOW         TRUE
)

# NcTools Executable
# We get a pre-built nc-convert so we don't have to build it and its dependencies
if(UNIX AND NOT APPLE)
    FetchContent_Declare(nc-convert
                         URL                        https://github.com/NcStudios/NcTools/releases/download/v2.0.0/nc-convert-ubuntu22.04-x64.zip
                         URL_HASH                   SHA256=ae96abf6349e42d3e5ee06a6431dddf8ae7950ab70d058ed4c8747fb8ee67501
                         DOWNLOAD_NO_EXTRACT        FALSE
                         DOWNLOAD_EXTRACT_TIMESTAMP TRUE
    )
elseif(WIN32)
    FetchContent_Declare(nc-convert
                         URL                        https://github.com/NcStudios/NcTools/releases/download/v2.0.0/nc-convert-windows-x64.zip
                         URL_HASH                   SHA256=97f0fe700224c5c9ab34d4538c1d85a89d6bd377f1040a1bff0b496c395edf08
                         DOWNLOAD_NO_EXTRACT        FALSE
                         DOWNLOAD_EXTRACT_TIMESTAMP TRUE
    )
elseif(APPLE)
    # TODO: #349 Artifact not yet published
endif()

# Taskflow
FetchContent_Declare(taskflow
                     GIT_REPOSITORY https://github.com/NcStudios/taskflow.git
                     GIT_TAG        origin/master
                     GIT_SHALLOW    TRUE
)

# GLFW
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

FetchContent_Declare(glfw
                     GIT_REPOSITORY https://github.com/NcStudios/glfw.git
                     GIT_TAG        origin/master
                     GIT_SHALLOW    TRUE
)

# Optick
if(${NC_PROFILING_ENABLED})
    set(OPTICK_ENABLED ON CACHE BOOL "" FORCE)
else()
    set(OPTICK_ENABLED OFF CACHE BOOL "" FORCE)
endif()

FetchContent_Declare(optick
                     GIT_REPOSITORY https://github.com/NcStudios/optick.git
                     GIT_TAG        e45a8480babaca85bd87ecf1e775547e28754881 # origin/release
                     GIT_SHALLOW    TRUE
)

# Fetch all required sources
FetchContent_MakeAvailable(NcCommon nc-tools nc-convert taskflow glfw optick)

# Set Taskflow includes as system to prevent some warnings
get_target_property(_Taskflow_Include_Prop Taskflow INTERFACE_INCLUDE_DIRECTORIES)
target_include_directories(Taskflow SYSTEM INTERFACE ${_Taskflow_Include_Prop})

#############################
### Optional Dependencies ###
#############################

# gtest
if(${NC_BUILD_TESTS})
    set(BUILD_GMOCK OFF CACHE BOOL "" FORCE)
    set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)

    FetchContent_Declare(googletest
                        GIT_REPOSITORY https://github.com/NcStudios/googletest.git
                        GIT_TAG        origin/main
    )

    FetchContent_MakeAvailable(googletest)
endif()
