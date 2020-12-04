cmake_minimum_required(VERSION 3.10)

project("NC Engine" VERSION "0.1.0" LANGUAGES CXX)

if(NOT EXISTS ${PROJECT_SOURCE_DIR}/build)
    file(MAKE_DIRECTORY ${PROJECT_SOURCE_DIR}/build)
endif()

#OPTIONS
option(DEBUG_BUILD "" ON)
option(NC_TESTS_ENABLED "Include NcEngine tests" OFF)
option(NC_EDITOR_ENABLED "Include in-game editor(also dear imgui)" ON)

#VARIABLES
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(GAME_EXE "Caverna")
set(SERVER_EXE "CavernaServer")
set(ROOT_DIR "${PROJECT_SOURCE_DIR}")
set(GCC_WARNING_FLAGS 
    -Wall
    -Wextra
    -Wfatal-errors
    -Wno-cast-function-type)
set(GCC_DEBUG_FLAGS "-g")
set(WIN32_LINK_FLAGS "-lgdi32 -ld3d11 -ld3dcompiler_47")
set(EXTERNAL_INCLUDE_DIR ${ROOT_DIR}/external/include)
set(PROJECT_INCLUDE_DIR
    ${ROOT_DIR}/project
    ${ROOT_DIR}/project/components
    ${ROOT_DIR}/project/models
    ${ROOT_DIR}/project/scenes${ROOT_DIR}/project/settings
)
set(NC_INCLUDE_DIRS
    ${ROOT_DIR}
    ${ROOT_DIR}/nc
    ${ROOT_DIR}/nc/include
    ${ROOT_DIR}/nc/source
)
set(EXTERNAL_SOURCE_DIR ${ROOT_DIR}/external/src)
set(EXTERNAL_LIB_DIR ${ROOT_DIR}/external/lib)

if(NC_TESTS_ENABLED)
    enable_testing()
endif()

### TARGETS ###
add_executable(${GAME_EXE})
add_executable(${SERVER_EXE})

#Propogate version
configure_file(${ROOT_DIR}/nc/source/config/Version.h.in
               ${ROOT_DIR}/nc/source/config/Version.h
)

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

if(DEBUG_BUILD)
    add_definitions(-DDEBUG_BUILD)
endif()

if(VERBOSE_LOGGING_ENABLED)
    add_definitions(-DVERBOSE_LOGGING_ENABLED)
endif()

#Compile Flags
target_compile_options(${GAME_EXE}
    PUBLIC
        ${GCC_WARNING_FLAGS}
)

target_compile_options(${SERVER_EXE}
    PUBLIC
        ${GCC_WARNING_FLAGS}
)

if(DEBUG_BUILD)
  target_compile_options(${GAME_EXE}
      PUBLIC
          ${GCC_DEBUG_FLAGS})
    target_compile_options(${SERVER_EXE}
        PUBLIC
            ${GCC_DEBUG_FLAGS})
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

#LINK OPTIONS
if(NC_EDITOR_ENABLED)
    target_link_libraries(${GAME_EXE}
        PRIVATE
            ${EXTERNAL_LIB_DIR}/libimgui.a
    )
endif()

target_link_libraries(${GAME_EXE}
    PRIVATE
        ${EXTERNAL_LIB_DIR}/windowscodecs.lib
        ${EXTERNAL_LIB_DIR}/libassimp.dll.a
        ${EXTERNAL_LIB_DIR}/libenet.a
        ${WIN32_LINK_FLAGS}
        Winmm.lib
        ws2_32
)

target_link_libraries(${SERVER_EXE}
    PRIVATE
        ${EXTERNAL_LIB_DIR}/libenet.a
        ${WIN32_LINK_FLAGS}
        winmm
        ws2_32
)

#SUBDIRS
add_subdirectory(nc/source)
add_subdirectory(project)

if(NC_TESTS_ENABLED)
  add_subdirectory(external/src/gtest)
  add_subdirectory(test)
endif()