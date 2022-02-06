cmake_minimum_required(VERSION 3.20)
project("Sample" LANGUAGES CXX)

if(${PROJECT_SOURCE_DIR} EQUAL ${PROJECT_BINARY_DIR})
    message(FATAL_ERROR "Don't be a fool, out-of-source build with your tool.")
endif()

if(NOT DEFINED NC_INSTALL_DIR)
    message(FATAL_ERROR "NC_INSTALL_DIR is not defined.")
endif()

#Variables
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(GAME_EXE "Sample")
set(ROOT_DIR "${PROJECT_SOURCE_DIR}")

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set(WARNING_FLAGS -Werror -Wall -Wextra -Wfatal-errors -Wno-cast-function-type)
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    set(WARNING_FLAGS /W3 /WX)
endif()

find_package(NcEngine REQUIRED PATHS ${NC_INSTALL_DIR})

add_executable(${GAME_EXE})

#Properties
set_target_properties(${GAME_EXE}
    PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY $<BUILD_INTERFACE:${ROOT_DIR}/..>
        #WIN32_EXECUTABLE TRUE
)

#Compile Options
target_compile_options(${GAME_EXE}
    PRIVATE
        ${WARNING_FLAGS}
)

#Include Dirs
target_include_directories(${GAME_EXE}
    PRIVATE
        ${PROJECT_SOURCE_DIR}/source
)

#Link Options
target_link_libraries(${GAME_EXE}
    PRIVATE
        Nc::NcEngineLib
)

target_sources(${GAME_EXE}
    PRIVATE
        ${ROOT_DIR}/SampleMain.cpp
)

add_subdirectory(source/scenes)
add_subdirectory(source/shared)