cmake_minimum_required(VERSION 3.20)
project("NcSdk" VERSION "0.1.0" LANGUAGES CXX C)

if(${PROJECT_SOURCE_DIR} EQUAL ${PROJECT_BINARY_DIR})
    message(FATAL_ERROR "Don't be a fool, out-of-source build with your tool.")
endif()

find_package(Vulkan REQUIRED)

# Options
option(NC_PROD_BUILD "Only build NcEngine production binaries." OFF)

# Variables
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_DEBUG_POSTFIX d)
set(NC_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/sdk/engine/include")
set(NC_SOURCE_DIR "${PROJECT_SOURCE_DIR}/sdk/engine/source")
set(NC_EXTERNAL_DIR "${PROJECT_SOURCE_DIR}/sdk/external")

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set(WARNING_FLAGS -Werror -Wall -Wextra -Wfatal-errors -Wno-cast-function-type)
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    set(WARNING_FLAGS /W3 /WX)
endif()

if(NC_PROD_BUILD)
    set(NC_ENGINE_LIB "NcEngine")
else()
    set(NC_COMPILE_DEFINITIONS NC_EDITOR_ENABLED NC_DEBUG_RENDERING_ENABLED NC_ASSERT_ENABLED NC_VERBOSE_LOGGING_ENABLED) #NC_PROFILING_ENABLED
    set(NC_ENGINE_LIB "NcEngine-dev")
    set(NC_EDITOR_EXE "NcEditor")
    set(NC_SAMPLE_EXE "Sample")
endif()
set(IMGUI_LIB "imgui")
set(ENET_LIB "enet")

add_subdirectory(sdk)

# Install
set(CMAKE_INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX}/NcSdk-${PROJECT_VERSION})

install(TARGETS ${NC_ENGINE_LIB} ${IMGUI_LIB} ${ENET_LIB}
        EXPORT NcEngine-export
        DESTINATION NcEngine/lib)

if(NOT NC_PROD_BUILD)
    install(TARGETS ${NC_EDITOR_EXE} EXPORT NcEngine-export DESTINATION NcEditor)
    install(TARGETS ${NC_SAMPLE_EXE} EXPORT NcEngine-export DESTINATION Sample)
endif()

install(EXPORT NcEngine-export
        FILE NcEngineTargets.cmake
        NAMESPACE Nc::
        DESTINATION NcEngine/lib/cmake/NcEngine)

# Install NcEngine Files
install(DIRECTORY ${PROJECT_SOURCE_DIR}/sdk/engine/include DESTINATION NcEngine)
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/NcEngineConfig.cmake DESTINATION NcEngine/lib/cmake/NcEngine)

# Install External Files
install(DIRECTORY ${PROJECT_SOURCE_DIR}/sdk/external/enet
                  ${PROJECT_SOURCE_DIR}/sdk/external/imgui
                  ${PROJECT_SOURCE_DIR}/sdk/external/taskflow/taskflow
                  ${PROJECT_SOURCE_DIR}/sdk/external/vulkan
        DESTINATION NcEngine/external
        PATTERN "src" EXCLUDE)
install(DIRECTORY ${PROJECT_SOURCE_DIR}/sdk/external/directx/Inc
        DESTINATION NcEngine/external/directx)

if(NOT NC_PROD_BUILD)
    # Install NcTools
    install(FILES ${PROJECT_SOURCE_DIR}/sdk/tools/asset_builder/NcAssetBuilder.exe
                  ${PROJECT_SOURCE_DIR}/sdk/tools/asset_builder/libassimp.dll
                  ${PROJECT_SOURCE_DIR}/sdk/tools/shaders.ps1
            DESTINATION NcTools)

    # Install NcEditor Files
    install(DIRECTORY ${PROJECT_SOURCE_DIR}/sdk/editor/file_templates DESTINATION NcEditor)
    install(FILES ${PROJECT_SOURCE_DIR}/sdk/editor/config/engine_config.ini DESTINATION NcEditor)

    # Install Sample Files
    install(DIRECTORY ${PROJECT_SOURCE_DIR}/sdk/sample/assets DESTINATION Sample)
    install(FILES ${PROJECT_SOURCE_DIR}/sdk/sample/config.ini DESTINATION Sample)
    if(${NC_PROFILING_ENABLED})
        install(FILES ${PROJECT_SOURCE_DIR}/sdk/engine/lib/OptickCore.dll DESTINATION Sample)
    endif()
endif()
