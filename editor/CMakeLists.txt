cmake_minimum_required(VERSION 3.10)
project("NcEditor" LANGUAGES CXX)

if(${PROJECT_SOURCE_DIR} EQUAL ${PROJECT_BINARY_DIR})
    message(FATAL_ERROR "Don't be a fool, out-of-source build with your tool.")
endif()

if(NOT DEFINED NC_BUILD_TYPE)
    message(FATAL_ERROR "NC_BUILD_TYPE is not defined. This is required in NcEngine and all projects for archive selection.")
endif()

if(NOT DEFINED NC_INSTALL_DIR)
    message(FATAL_ERROR "NC_INSTALL_DIR is not defined.")
endif()

#Variables
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(EDITOR_EXE "NcEditor")

set(WARNING_FLAGS)

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set(WARNING_FLAGS -Wall -Wextra -Wfatal-errors -Wno-cast-function-type)
elseif()
endif()

find_package(NcEngine REQUIRED PATHS ${NC_INSTALL_DIR})

add_executable(${EDITOR_EXE})

#Properties
set_target_properties(${EDITOR_EXE}
    PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/..
)

#Compile Options
target_compile_options(${EDITOR_EXE}
    PUBLIC
        ${WARNING_FLAGS}
)

#Include Dirs
target_include_directories(${EDITOR_EXE}
    PRIVATE
        ${PROJECT_SOURCE_DIR}/source
)

#Link Options
target_link_libraries(${EDITOR_EXE}
    PRIVATE
        Nc::NcEngineLib
)

target_sources(${EDITOR_EXE}
    PRIVATE
        ${PROJECT_SOURCE_DIR}/source/main.cpp
)

add_subdirectory(source)