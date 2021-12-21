cmake_minimum_required(VERSION 3.10)
project("NcEngine" VERSION "0.1.0" LANGUAGES CXX C)

if(${PROJECT_SOURCE_DIR} EQUAL ${PROJECT_BINARY_DIR})
    message(FATAL_ERROR "Don't be a fool, out-of-source build with your tool.")
endif()

if(NOT DEFINED NC_BUILD_TYPE)
    message(FATAL_ERROR "NC_BUILD_TYPE is not defined. This is required in NcEngine and all projects for archive selection.")
endif()

if(NOT DEFINED NC_ENGINE_INSTALL_DIR)
    message(FATAL_ERROR "NC_ENGINE_INSTALL_DIR is not defined.")
endif()

#OPTIONS
option(NC_TESTS_ENABLED "Include NcEngine tests" OFF)
option(NC_EDITOR_ENABLED "Include in-game editor(also dear imgui)" OFF)
option(NC_DEBUG_RENDERING "Enable wireframe primitive rendering" OFF)
option(NC_USE_VALIDATION "Use Vulkan validation layers" OFF)
option(NC_PROFILING_ENABLED "Include Optick profiling" OFF)

#VARIABLES
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(ROOT_DIR "${PROJECT_SOURCE_DIR}")
set(LIB_DIR "${ROOT_DIR}/lib")
set(NC_INCLUDE_DIR "${ROOT_DIR}/include")
set(NC_SOURCE_DIR "${ROOT_DIR}/source")
set(NC_EXTERNAL_DIR "${ROOT_DIR}/external")
set(WIN32_LINK_FLAGS "-lgdi32 -ld3d11 -ld3dcompiler_47 -lwinmm -lws2_32")
set(WIN32_DEFINITIONS -DWIN32_LEAN_AND_MEAN)
set(RTAUDIO_LINK_FLAGS "-lole32 -lwinmm -lksuser -lmfplat -lmfuuid -lwmcodecdspuuid")
set(ENGINE_LIB "NcEngineLib")
set(IMGUI_LIB "imgui-${CMAKE_BUILD_TYPE}")
set(ENET_LIB "enet-${CMAKE_BUILD_TYPE}")
set(NC_INSTALL_DIR "${NC_ENGINE_INSTALL_DIR}/NcEngine/${NC_BUILD_TYPE}")

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set(WARNING_FLAGS -Wall -Wextra -Wfatal-errors -Wno-cast-function-type)
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
endif()

# Propagate version
configure_file(${ROOT_DIR}/source/config/Version.h.in
               ${ROOT_DIR}/include/config/Version.h
)

# TARGETS
add_library(NcEngineLib STATIC)

# Definitions
target_compile_definitions(NcEngineLib
    PUBLIC
        WIN32_LEAN_AND_MEAN
        $<INSTALL_INTERFACE:IMGUI_USER_CONFIG="ui/DearImGuiConversion.h">
        $<BUILD_INTERFACE:IMGUI_USER_CONFIG="${PROJECT_SOURCE_DIR}/include/ui/DearImGuiConversion.h">
    PRIVATE
        __WINDOWS_WASAPI__
)

if(NC_EDITOR_ENABLED)
    target_compile_definitions(NcEngineLib PUBLIC NC_EDITOR_ENABLED)
endif()

if(NC_DEBUG_RENDERING)
    target_compile_definitions(NcEngineLib PUBLIC NC_DEBUG_RENDERING)
endif()

if(NC_PROFILING_ENABLED)
    target_compile_definitions(NcEngineLib PUBLIC NC_PROFILING_ENABLED)
endif()

if(NC_USE_VALIDATION)
    target_compile_definitions(NcEngineLib PUBLIC NC_USE_VALIDATION)
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(NcEngineLib PUBLIC NC_DEBUG_BUILD)
    add_definitions(-DNC_DEBUG_BUILD)
endif()

if(VERBOSE_LOGGING_ENABLED)
    target_compile_definitions(NcEngineLib PUBLIC VERBOSE_LOGGING_ENABLED)
endif()

# Properties
set_target_properties(NcEngineLib
    PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY $<BUILD_INTERFACE:${LIB_DIR}>
)

# Compile Options
target_compile_options(NcEngineLib
    PRIVATE
        ${WARNING_FLAGS}
)

add_subdirectory(external/enet/src)
add_subdirectory(external/imgui/src)

find_package(Vulkan REQUIRED)

# Link libraries
target_link_libraries(NcEngineLib
    INTERFACE
        ${IMGUI_LIB}
        ${RTAUDIO_LINK_FLAGS}
        ${WIN32_LINK_FLAGS}
        #{ENET_LIB} add for project requiring networking
    PUBLIC
        Vulkan::Vulkan
)

# Note - libOptickCore.dll still needs to be copied to the executable's directory
if(${NC_PROFILING_ENABLED})
    target_link_libraries(NcEngineLib
        INTERFACE
            $<INSTALL_INTERFACE:${NC_INSTALL_DIR}/lib/libOptickCore.a>
    )
endif()

# Include Dirs
target_include_directories(NcEngineLib
    PUBLIC
        $<INSTALL_INTERFACE:include/>
        $<INSTALL_INTERFACE:external/>
    PRIVATE
        $<BUILD_INTERFACE:${NC_INCLUDE_DIR}>
        $<BUILD_INTERFACE:${NC_SOURCE_DIR}>
        $<BUILD_INTERFACE:${NC_EXTERNAL_DIR}>
)

add_subdirectory(source)

if(NC_TESTS_ENABLED)
    enable_testing()
    add_subdirectory(external/gtest)
    add_subdirectory(test)
endif()

set(CMAKE_INSTALL_PREFIX ${NC_INSTALL_DIR})
install(TARGETS NcEngineLib ${IMGUI_LIB} ${ENET_LIB}
        EXPORT NcEngine-export
        DESTINATION lib
)

install(EXPORT NcEngine-export
    FILE NcEngineTargets.cmake
    NAMESPACE Nc::
    DESTINATION lib/cmake/NcEngine
)

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/ DESTINATION include)
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/external/directx/Inc DESTINATION external/directx/)
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/external/enet
        DESTINATION external/
        PATTERN "src" EXCLUDE
)
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/external/imgui
        DESTINATION external/
        PATTERN "src" EXCLUDE
)
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/external/taskflow/taskflow DESTINATION external/)
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/external/vulkan DESTINATION external/)
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/NcEngineConfig.cmake DESTINATION lib/cmake/NcEngine)

if(${NC_PROFILING_ENABLED})
    install(FILES ${LIB_DIR}/libOptickCore.a DESTINATION lib)
endif()