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
set(GCC_WARNING_FLAGS -Wall -Wextra -Wno-cast-function-type)
set(GCC_DEBUG_FLAGS "-g")
set(WIN32_LINK_FLAGS "-lgdi32 -ld3d11 -ld3dcompiler_47")
set(PROJECT_INCLUDE_DIRS ${ROOT_DIR}/project ${ROOT_DIR}/project/components ${ROOT_DIR}/project/models ${ROOT_DIR}/project/scenes ${ROOT_DIR}/project/settings)
set(NC_COMMON_DIRS ${ROOT_DIR} ${ROOT_DIR}/nc ${ROOT_DIR}/nc/component ${ROOT_DIR}/nc/input ${ROOT_DIR}/nc/math ${ROOT_DIR}/nc/scene ${ROOT_DIR}/nc/time ${ROOT_DIR}/nc/views)


### TARGET imgui-lib ###
add_library(imgui-lib
            STATIC
            ${ROOT_DIR}/external/imgui/imgui_impl_dx11.cpp
            ${ROOT_DIR}/external/imgui/imgui_impl_win32.cpp
            ${ROOT_DIR}/external/imgui/imgui.cpp
            ${ROOT_DIR}/external/imgui/imgui_demo.cpp
            ${ROOT_DIR}/external/imgui/imgui_draw.cpp
            ${ROOT_DIR}/external/imgui/imgui_widgets.cpp
)
target_include_directories(imgui-lib PUBLIC ${NC_COMMON_DIRS})
target_link_libraries(imgui-lib PRIVATE "-ld3dcompiler_47")


### TARGET GAME.EXE ###
add_executable(${EXE_NAME})
set_target_properties(${EXE_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR})

#COMPILE OPTIONS
target_compile_options(${EXE_NAME} PUBLIC ${GCC_WARNING_FLAGS})
if(DEBUG_BUILD)
  add_definitions(-DNC_DEBUG)
  target_compile_options(${EXE_NAME} PUBLIC ${GCC_DEBUG_FLAGS})
endif()

#LINK OPTIONS
target_link_libraries(${EXE_NAME} PUBLIC ${WIN32_LINK_FLAGS} imgui-lib)

#target_precompile_headers(${EXE_NAME} PUBLIC ${ROOT_DIR}/nc/PCH.h)

target_include_directories(${EXE_NAME}
                             PUBLIC
                               ${ROOT_DIR}
                               ${NC_COMMON_DIRS}
                               ${PROJECT_INCLUDE_DIRS}
)



#target_sources(${EXE_NAME}
#                   PUBLIC
#                       #editor/imgui directory
#                       
#)

add_subdirectory(nc)
add_subdirectory(project)
add_subdirectory(external/gtest)
add_subdirectory(test)