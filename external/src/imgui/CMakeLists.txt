cmake_minimum_required(VERSION 3.10)

project("Dear ImGui" LANGUAGES CXX)

if(${PROJECT_SOURCE_DIR} EQUAL ${PROJECT_BINARY_DIR})
    message(FATAL_ERROR "Don't be fool, out-of-source build with your tool.")
endif()

#VARIABLES
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(LIB_NAME "imgui-${CMAKE_BUILD_TYPE}")
set(ROOT_DIR "${PROJECT_SOURCE_DIR}")
set(GCC_WARNING_FLAGS 
    -Wall
    -Wextra
    -Wfatal-errors
    -Wno-cast-function-type)

set(EXTERNAL_INCLUDE_DIR ${ROOT_DIR}/../../include)
set(NC_COMMON_INCLUDE_DIR  ${ROOT_DIR}/../../../nc)

set(EXTERNAL_LIB_DIR ${ROOT_DIR}/../../lib)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${EXTERNAL_LIB_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${EXTERNAL_LIB_DIR})

###Imgui Library###
add_library(${LIB_NAME}
    STATIC
        ${PROJECT_SOURCE_DIR}/imgui_impl_dx11.cpp
        ${PROJECT_SOURCE_DIR}/imgui_impl_win32.cpp
        ${PROJECT_SOURCE_DIR}/imgui.cpp
        ${PROJECT_SOURCE_DIR}/imgui_demo.cpp
        ${PROJECT_SOURCE_DIR}/imgui_draw.cpp
        ${PROJECT_SOURCE_DIR}/imgui_widgets.cpp
)

target_include_directories(${LIB_NAME}
    PRIVATE
        ${NC_COMMON_INCLUDE_DIR}
        ${EXTERNAL_INCLUDE_DIR}
)

target_link_libraries(${LIB_NAME}
    PRIVATE
        "-ld3dcompiler_47"
)