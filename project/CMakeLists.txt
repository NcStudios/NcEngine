cmake_minimum_required(VERSION 3.10)
project("Caverna" LANGUAGES CXX)

if(${PROJECT_SOURCE_DIR} EQUAL ${PROJECT_BINARY_DIR})
    message(FATAL_ERROR "Don't be fool, out-of-source build with your tool.")
endif()

#Options
option(NC_TESTS_ENABLED "Include NcEngine tests" OFF)
option(NC_EDITOR_ENABLED "Include in-game editor(also dear imgui)" OFF)
option(PROFILING_ENABLED "Include gprof instrumentation" OFF)
option(SANITIZE_UB "Use undefined behavior sanitizer" OFF)

#Variables
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(GAME_EXE "Caverna")
set(SERVER_EXE "CavernaServer")
set(ROOT_DIR "${PROJECT_SOURCE_DIR}/..")
set(GCC_WARNING_FLAGS 
    -Wall
    -Wextra
    -Wfatal-errors
    -Wno-cast-function-type
)
set(WIN32_LINK_FLAGS "-lgdi32 -ld3d11 -ld3dcompiler_47 -lwinmm -lws2_32")
set(EXTERNAL_INCLUDE_DIR ${ROOT_DIR}/external/include)
set(PROJECT_INCLUDE_DIR
    ${ROOT_DIR}/project
    ${ROOT_DIR}/project/components
    ${ROOT_DIR}/project/models
    ${ROOT_DIR}/project/scenes
    ${ROOT_DIR}/project/settings
)
set(NC_INCLUDE_DIRS
    ${ROOT_DIR}
    ${ROOT_DIR}/nc
    ${ROOT_DIR}/nc/include
    ${ROOT_DIR}/nc/source
)

set(EXTERNAL_LIB_DIR ${ROOT_DIR}/external/lib)
set(LIB_IMGUI "libimgui-${CMAKE_BUILD_TYPE}.a")
set(LIB_ENET "libenet-${CMAKE_BUILD_TYPE}.a")

add_executable(${GAME_EXE})
add_executable(${SERVER_EXE})

#Properties
set_target_properties(${GAME_EXE}
    PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${ROOT_DIR}
)

set_target_properties(${SERVER_EXE}
    PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${ROOT_DIR}
)

#Definitions
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
target_include_directories(${GAME_EXE}
    PRIVATE
        ${NC_INCLUDE_DIRS}
        ${PROJECT_INCLUDE_DIR}
        ${EXTERNAL_INCLUDE_DIR}
)

target_include_directories(${SERVER_EXE}
    PRIVATE
        ${ROOT_DIR}
        ${PROJECT_INCLUDE_DIR}
        ${EXTERNAL_INCLUDE_DIR}
)

#Compile Options
target_compile_options(${GAME_EXE}
    PUBLIC
        ${GCC_WARNING_FLAGS}
)

target_compile_options(${SERVER_EXE}
    PUBLIC
        ${GCC_WARNING_FLAGS}
)

if(SANITIZE_UB)
    target_compile_options(${GAME_EXE}
        PUBLIC
            -fsanitize=undefined
            -fsanitize-undefined-trap-on-error
    )
endif()

if(PROFILING_ENABLED)
    target_compile_options(${GAME_EXE}
        PUBLIC
            -pg
    )
endif()

#Link Options
target_link_libraries(${GAME_EXE}
    PRIVATE
        ${ROOT_DIR}/nc/lib/libNcEngine-${CMAKE_BUILD_TYPE}.a
        ${EXTERNAL_LIB_DIR}/${LIB_IMGUI}
        ${EXTERNAL_LIB_DIR}/windowscodecs.lib
        ${EXTERNAL_LIB_DIR}/libassimp.dll.a
        ${EXTERNAL_LIB_DIR}/${LIB_ENET}
        ${WIN32_LINK_FLAGS}
)

if(PROFILING_ENABLED)
    target_link_options(${GAME_EXE}
        PUBLIC
            -pg
    )
endif()

target_link_libraries(${SERVER_EXE}
    PRIVATE
        ${ROOT_DIR}/nc/lib/libNcEngine-${CMAKE_BUILD_TYPE}.a
        ${EXTERNAL_LIB_DIR}/${LIB_ENET}
        ${WIN32_LINK_FLAGS}
)

#Sources
file(GLOB USER_COMPONENT_SOURCE ${ROOT_DIR}/project/components/*.cpp)
file(GLOB USER_SCENE_SOURCE ${ROOT_DIR}/project/scenes/*.cpp)
file(GLOB USER_UI_SOURCE ${ROOT_DIR}/project/ui/*.cpp)

target_sources(${GAME_EXE}
    PRIVATE
        ${ROOT_DIR}/project/CavernaMain.cpp
        ${USER_COMPONENT_SOURCE}
        ${USER_SCENE_SOURCE}
        ${USER_UI_SOURCE}
)

target_sources(${SERVER_EXE}
    PRIVATE
        ${ROOT_DIR}/project/ServerMain.cpp)

add_subdirectory(config)
add_subdirectory(source)