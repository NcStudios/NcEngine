cmake_minimum_required(VERSION 3.10)
project("NcEditor" LANGUAGES CXX)

if(${PROJECT_SOURCE_DIR} EQUAL ${PROJECT_BINARY_DIR})
    message(FATAL_ERROR "Don't be a fool, out-of-source build with your tool.")
endif()

#Options
option(NC_EDITOR_ENABLED "Include in-game editor(also dear imgui)" OFF)

#Variables
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(EDITOR_EXE "NcEditor")
set(ROOT_DIR "${PROJECT_SOURCE_DIR}")
set(GCC_WARNING_FLAGS 
    -Wall
    -Wextra
    -Wfatal-errors
    -Wno-cast-function-type
)
set(WIN32_LINK_FLAGS "-lgdi32 -ld3d11 -ld3dcompiler_47 -lws2_32")
set(WIN32_DEFINITIONS -DWIN32_LEAN_AND_MEAN)
set(EXTERNAL_INCLUDE_DIR ${ROOT_DIR}/../nc/external/include)
set(PROJECT_INCLUDE_DIR ${ROOT_DIR}/source)
set(NC_INCLUDE_DIR ${ROOT_DIR}/../nc/include)
set(RTAUDIO_LINK_FLAGS "-lole32 -lwinmm -lksuser -lmfplat -lmfuuid -lwmcodecdspuuid")

set(LIB_DIR ${ROOT_DIR}/../nc/lib)
set(ENGINE_LIB "libNcEngine-${CMAKE_BUILD_TYPE}.a")
set(IMGUI_LIB "libimgui-${CMAKE_BUILD_TYPE}.a")
set(ENET_LIB "libenet-${CMAKE_BUILD_TYPE}.a")

add_executable(${EDITOR_EXE})

#Properties
set_target_properties(${EDITOR_EXE}
    PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${ROOT_DIR}/..
)

#Definitions
add_definitions(${WIN32_DEFINITIONS})

if(NC_EDITOR_ENABLED)
    add_definitions(-DNC_EDITOR_ENABLED)
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_definitions(-DNC_DEBUG_BUILD)
endif()

if(VERBOSE_LOGGING_ENABLED)
    add_definitions(-DVERBOSE_LOGGING_ENABLED)
endif()

#Include Dirs
target_include_directories(${EDITOR_EXE}
    PRIVATE
        ${NC_INCLUDE_DIR}
        ${PROJECT_INCLUDE_DIR}
        ${EXTERNAL_INCLUDE_DIR}
)

#Compile Options
target_compile_options(${EDITOR_EXE}
    PUBLIC
        ${GCC_WARNING_FLAGS}
)

find_package(Vulkan REQUIRED)

#Link Options
target_link_libraries(${EDITOR_EXE}
    PRIVATE
        ${LIB_DIR}/${ENGINE_LIB}
        ${LIB_DIR}/${IMGUI_LIB}
        ${LIB_DIR}/windowscodecs.lib
        ${WIN32_LINK_FLAGS}
        ${RTAUDIO_LINK_FLAGS}
)

target_link_libraries(${EDITOR_EXE}
    PRIVATE
        Vulkan::Vulkan
)

target_sources(${EDITOR_EXE}
    PRIVATE
        ${ROOT_DIR}/source/main.cpp
)

add_subdirectory(source)