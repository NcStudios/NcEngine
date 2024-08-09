#############################
### Required Dependencies ###
#############################

include(FetchContent)

function(file_exists FILE)
    if(EXISTS ${FILE})
        set(file_exists_RESULT TRUE PARENT_SCOPE)
    else()
        set(file_exists_RESULT FALSE PARENT_SCOPE)
    endif()
endfunction()

# Vulkan
find_package(Vulkan REQUIRED)

# Vulkan Memory Allocator
set(_DEFAULT_SDK_VMA_PATH "${Vulkan_INCLUDE_DIR}/vma/vk_mem_alloc.h")

file_exists(${_DEFAULT_SDK_VMA_PATH})
set(_DEFAULT_SDK_VMA_EXISTS ${file_exists_RESULT})

if(NC_VMA_PATH)
    file_exists(${NC_VMA_PATH})
    set(_OVERRIDE_VMA_EXISTS ${file_exists_RESULT})
else()
    set(_OVERRIDE_VMA_EXISTS FALSE)
endif()

if(_OVERRIDE_VMA_EXISTS)
    message(STATUS "Using user-specified VMA path: ${NC_VMA_PATH}")
    set(VMA_HEADER_PATH ${NC_VMA_PATH})
elseif(_DEFAULT_SDK_VMA_EXISTS)
    message(STATUS "Using VMA from Vulkan SDK: ${_DEFAULT_SDK_VMA_PATH}")
    set(VMA_HEADER_PATH ${_DEFAULT_SDK_VMA_PATH})
else()
    message(STATUS "Using vendored fallback VMA header")
    set(VMA_HEADER_PATH "${CMAKE_SOURCE_DIR}/source/external/vma_fallback/vk_mem_alloc.h")
endif()

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
                     GIT_TAG             v3.1.0-ncasset
                     GIT_SHALLOW         TRUE
)

# NcTools Executable
# We get a pre-built nc-convert so we don't have to build it and its dependencies
if(UNIX AND NOT APPLE)
    FetchContent_Declare(nc-convert
                         URL                        https://github.com/NcStudios/NcTools/releases/download/v3.0.0/nc-convert-ubuntu22.04-x64.zip
                         URL_HASH                   SHA256=5B9AAF2597001CC2A4052239F2D50918A274D258506B1BC92DEEDC15D80FC563
                         DOWNLOAD_NO_EXTRACT        FALSE
                         DOWNLOAD_EXTRACT_TIMESTAMP FALSE
    )
elseif(WIN32)
    FetchContent_Declare(nc-convert
                         URL                        https://github.com/NcStudios/NcTools/releases/download/v3.0.0/nc-convert-windows-x64.zip
                         URL_HASH                   SHA256=B94D0A4C29A7B44085267583D424B04DFA2AAB684AB24CCFADB67C94A4C63846
                         DOWNLOAD_NO_EXTRACT        FALSE
                         DOWNLOAD_EXTRACT_TIMESTAMP FALSE
    )
elseif(APPLE)
    # TODO: #349 Artifact not yet published
endif()

# Taskflow
set(TF_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(TF_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

FetchContent_Declare(taskflow
                     GIT_REPOSITORY https://github.com/taskflow/taskflow.git
                     GIT_TAG        v3.6.0
                     GIT_SHALLOW    TRUE
)

# GLFW
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

FetchContent_Declare(glfw
                     GIT_REPOSITORY https://github.com/glfw/glfw.git
                     GIT_TAG        3.3.9
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
                     GIT_TAG        v1.3.0+nc.1
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
                         GIT_TAG        v1.14.0+nc.1
                         GIT_SHALLOW    TRUE
    )

    FetchContent_MakeAvailable(googletest)
endif()
