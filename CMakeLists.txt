cmake_minimum_required(VERSION 3.10)

project("NC Engine" LANGUAGES CXX)

if(${PROJECT_SOURCE_DIR} EQUAL ${PROJECT_BINARY_DIR})
    message(FATAL_ERROR "Don't be fool, out-of-source build with your tool.")
endif()

#VARIABLES
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(EXE_NAME "app")
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
set(NC_COMMON_INCLUDE_DIR
    ${ROOT_DIR} ${ROOT_DIR}/nc
    ${ROOT_DIR}/nc/component
    ${ROOT_DIR}/nc/input
    ${ROOT_DIR}/nc/math
    ${ROOT_DIR}/nc/scene
    ${ROOT_DIR}/nc/time
)
set(EXTERNAL_SOURCE_DIR ${ROOT_DIR}/external/src)
set(EXTERNAL_LIB_DIR ${ROOT_DIR}/external/lib)

#OPTIONS
option(DEBUG_BUILD "" ON)
option(NC_TESTS_ENABLED "Include NcEngine tests" OFF)
option(NC_EDITOR_ENABLED "Include in-game editor(also dear imgui)" ON)

### TARGET GAME.EXE ###
add_executable(${EXE_NAME})

#Properties
set_target_properties(${EXE_NAME}
    PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}
)

#Definitions
if(NC_EDITOR_ENABLED)
    add_definitions(-DNC_EDITOR_ENABLED)
endif()

#Compile Flags
target_compile_options(${EXE_NAME}
    PUBLIC
        ${GCC_WARNING_FLAGS}
)

if(NC_EDITOR_ENABLED)
  target_compile_options(${EXE_NAME}
      PUBLIC
          ${GCC_DEBUG_FLAGS})
endif()

#Include Dirs
target_include_directories(${EXE_NAME}
    PRIVATE
        ${ROOT_DIR}
        ${NC_COMMON_INCLUDE_DIR}
        ${PROJECT_INCLUDE_DIR}
        ${EXTERNAL_INCLUDE_DIR}
)

#LINK OPTIONS
if(NC_EDITOR_ENABLED)
    target_link_libraries(${EXE_NAME}
        PRIVATE
            ${EXTERNAL_LIB_DIR}/libimgui.a
    )
endif()

target_link_libraries(${EXE_NAME}
    PRIVATE
        ${WIN32_LINK_FLAGS}
)

#SUBDIRS
add_subdirectory(nc)
add_subdirectory(project)

if(NC_TESTS_ENABLED)
  add_subdirectory(external/src/gtest)
  add_subdirectory(test)
endif()