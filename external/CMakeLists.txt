cmake_minimum_required(VERSION 3.10)

project("External Libs" LANGUAGES CXX C)

if(${PROJECT_SOURCE_DIR} EQUAL ${PROJECT_BINARY_DIR})
    message(FATAL_ERROR "Don't be fool, out-of-source build with your tool.")
endif()

#VARIABLES
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(ROOT_DIR "${PROJECT_SOURCE_DIR}")
set(GCC_WARNING_FLAGS 
    -Wall
    -Wextra
    -Wfatal-errors
    -Wno-cast-function-type)
set(GCC_DEBUG_FLAGS "-g")

set(EXTERNAL_INCLUDE_DIR ${ROOT_DIR}/include)
set(NC_COMMON_INCLUDE_DIR  ${ROOT_DIR}/../nc)

set(EXTERNAL_SOURCE_DIR ${ROOT_DIR}/src)
set(EXTERNAL_LIB_DIR ${ROOT_DIR}/lib)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${EXTERNAL_LIB_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${EXTERNAL_LIB_DIR})

###Imgui Library###
add_library(imgui
    STATIC
        ${EXTERNAL_SOURCE_DIR}/imgui/imgui_impl_dx11.cpp
        ${EXTERNAL_SOURCE_DIR}/imgui/imgui_impl_win32.cpp
        ${EXTERNAL_SOURCE_DIR}/imgui/imgui.cpp
        ${EXTERNAL_SOURCE_DIR}/imgui/imgui_demo.cpp
        ${EXTERNAL_SOURCE_DIR}/imgui/imgui_draw.cpp
        ${EXTERNAL_SOURCE_DIR}/imgui/imgui_widgets.cpp
)

target_include_directories(imgui
    PRIVATE
        ${NC_COMMON_INCLUDE_DIR}
        ${EXTERNAL_INCLUDE_DIR}
)

target_link_libraries(imgui
    PRIVATE
        "-ld3dcompiler_47"
)

add_subdirectory(src/enet)