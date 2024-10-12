function(disable_warnings_for_target target)
    if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        target_compile_options(${target} PRIVATE -w)
    elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
        target_compile_options(${target} PRIVATE /W0)
    endif()
endfunction()

function(disable_warnings_for_headers target)
    get_target_property(target_include_dirs ${target} INTERFACE_INCLUDE_DIRECTORIES)
    if (target_include_dirs)
        target_include_directories(${target} SYSTEM INTERFACE ${target_include_dirs})
    endif()
endfunction()

include(FetchContent)

# Required Dependencies

# Vulkan
find_package(Vulkan REQUIRED)

# Vulkan Memory Allocator
if(EXISTS "${Vulkan_INCLUDE_DIR}/vma/vk_mem_alloc.h")
    message(STATUS "Using VMA from Vulkan SDK: ${Vulkan_INCLUDE_DIR}")
    set(VMA_INCLUDE_DIR ${Vulkan_INCLUDE_DIR})
else()
    message(STATUS "Using vendored fallback VMA header")
    set(VMA_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/source/external/vma_fallback")
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

# Jolt
set(CPP_EXCEPTIONS_ENABLED ON CACHE BOOL "" FORCE)
set(CPP_RTTI_ENABLED ON CACHE BOOL "" FORCE)
set(DEBUG_RENDERER_IN_DEBUG_AND_RELEASE OFF CACHE BOOL "" FORCE)
set(ENABLE_OBJECT_STREAM OFF CACHE BOOL "" FORCE)
set(PROFILER_IN_DEBUG_AND_RELEASE OFF CACHE BOOL "" FORCE)
FetchContent_Declare(JoltPhysics
                     GIT_REPOSITORY https://github.com/jrouwe/JoltPhysics
                     GIT_TAG        v5.1.0
                     GIT_SHALLOW    TRUE
                     SOURCE_SUBDIR  "Build"
)

# Diligent
set(DILIGENT_INSTALL_CORE OFF CACHE BOOL "" FORCE)
set(DILIGENT_INSTALL_TOOLS OFF CACHE BOOL "" FORCE)
set(DILIGENT_INSTALL_SAMPLES OFF CACHE BOOL "" FORCE)
set(DILIGENT_INSTALL_FX OFF CACHE BOOL "" FORCE)
set(DILIGENT_BUILD_SAMPLES OFF CACHE BOOL "" FORCE)
set(DILIGENT_NO_RENDER_STATE_PACKAGER OFF CACHE BOOL "" FORCE)
set(DILIGENT_NO_GLSLANG OFF CACHE BOOL "" FORCE)
set(DILIGENT_NO_OPENGL ON CACHE BOOL "" FORCE)
set(DILIGENT_NO_METAL ON CACHE BOOL "" FORCE)

list(APPEND DILIGENT_LIBRARIES
            Diligent-TargetPlatform
            Diligent-GraphicsEngineVk-shared
            Diligent-TextureLoader
)

if(WIN32)
    list(APPEND DILIGENT_LIBRARIES 
                Diligent-GraphicsEngineD3D11-shared
                Diligent-GraphicsEngineD3D12-shared
    )
endif()

FetchContent_Declare(DiligentCore
                     GIT_REPOSITORY https://github.com/DiligentGraphics/DiligentCore.git
                     GIT_TAG        v2.5.6
                     GIT_SHALLOW    TRUE
                     SOURCE_DIR     _deps/DiligentCore
)

FetchContent_Declare(DiligentTools
                     GIT_REPOSITORY https://github.com/DiligentGraphics/DiligentTools.git
                     GIT_TAG        v2.5.6
                     GIT_SHALLOW    TRUE
                     SOURCE_DIR     _deps/DiligentTools
)

# DirectXMath
FetchContent_Declare(DirectXMath
                     GIT_REPOSITORY https://github.com/NcStudios/DirectXMath.git
                     GIT_TAG        v3.1.6+nc.1
                     GIT_SHALLOW    TRUE
)

# fmt - we only need because GCC hasn't implemented std::format yet. Can be removed eventually.
set(FMT_INSTALL ON)
FetchContent_Declare(fmt
                     GIT_REPOSITORY https://github.com/fmtlib/fmt.git
                     GIT_TAG        10.1.1
                     GIT_SHALLOW    TRUE
)

# Fetch all required sources
FetchContent_MakeAvailable(taskflow glfw optick JoltPhysics DirectXMath fmt DiligentCore DiligentTools)

# Silence warnings
disable_warnings_for_headers(Taskflow)
disable_warnings_for_headers(DirectXMath)
disable_warnings_for_headers(fmt)
disable_warnings_for_headers(Jolt)

# Tell Jolt to use our profile implementation. This introduces a circular dependency between Jolt/NcEngine,
# which GCC struggles with (but it could be coerced), so we just exclude Jolt events from nix profiling.
if(NC_PROFILING_ENABLED AND NOT CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_compile_definitions(Jolt
        PUBLIC
            -DJPH_EXTERNAL_PROFILE
    )
endif()

# Optional Dependencies

# Assimp
if(NC_BUILD_NCCONVERT)
    set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
    set(ASSIMP_BUILD_ZLIB ON CACHE BOOL "" FORCE)
    set(ASSIMP_INSTALL OFF CACHE BOOL "" FORCE)
    set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
    set(ASSIMP_BUILD_ASSIMP_TOOLS OFF CACHE BOOL "" FORCE)
    set(ASSIMP_NO_EXPORT OFF CACHE BOOL "" FORCE)
    set(ASSIMP_BUILD_ALL_IMPORTERS_BY_DEFAULT OFF CACHE BOOL "" FORCE)
    set(ASSIMP_BUILD_ALL_EXPORTERS_BY_DEFAULT OFF CACHE BOOL "" FORCE)
    set(ASSIMP_BUILD_FBX_IMPORTER ON CACHE BOOL "" FORCE)
    set(ASSIMP_BUILD_OBJ_IMPORTER ON CACHE BOOL "" FORCE)
    set(ASSIMP_INSTALL_PDB OFF CACHE BOOL "" FORCE)

    FetchContent_Declare(assimp
                        GIT_REPOSITORY https://github.com/NcStudios/assimp.git
                        GIT_TAG        v5.3.0+nc.1
                        GIT_SHALLOW    TRUE
    )

    FetchContent_MakeAvailable(assimp)

    disable_warnings_for_headers(assimp)
    disable_warnings_for_target(assimp)
endif()

# gtest
if(${NC_BUILD_TESTS} OR ${NC_BUILD_LINUX_INTEGRATION_TESTS} OR ${NC_BUILD_WIN32_INTEGRATION_TESTS})
    set(BUILD_GMOCK OFF CACHE BOOL "" FORCE)
    set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)

    FetchContent_Declare(googletest
                         GIT_REPOSITORY https://github.com/NcStudios/googletest.git
                         GIT_TAG        v1.14.0+nc.1
                         GIT_SHALLOW    TRUE
    )

    FetchContent_MakeAvailable(googletest)
endif()
