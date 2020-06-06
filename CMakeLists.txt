cmake_minimum_required(VERSION 3.10)

project("NC Engine" 
           LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

if(${PROJECT_SOURCE_DIR} EQUAL ${PROJECT_BINARY_DIR})
    message(FATAL_ERROR "Don't be fool, out-of-source build with your tool.")
endif()

set(EXE_NAME "app")
set(ROOT_DIR "${PROJECT_SOURCE_DIR}")
set(GCC_WARNING_FLAGS -Wall -Wextra -Wno-cast-function-type)
set(GCC_DEBUG_FLAGS "-g")
set(WIN32_LINK_FLAGS "-lgdi32 -ld3d11 -ld3dcompiler_47")

add_executable(${EXE_NAME})

set_target_properties(${EXE_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR})

if(DEBUG_BUILD)
  add_definitions(-DNC_DEBUG)
  target_compile_options(${EXE_NAME} PUBLIC ${GCC_DEBUG_FLAGS})
endif()

target_compile_options(${EXE_NAME} PUBLIC ${GCC_WARNING_FLAGS})

target_link_libraries(${EXE_NAME} PUBLIC ${WIN32_LINK_FLAGS})

#target_precompile_headers(${EXE_NAME} PUBLIC ${ROOT_DIR}/nc/PCH.h)

#should have files use relative paths from root directory
target_include_directories(${EXE_NAME}
                             PUBLIC
                               ${ROOT_DIR}
                               ${ROOT_DIR}/nc
                               ${ROOT_DIR}/nc/component
                               ${ROOT_DIR}/nc/debug
                               ${ROOT_DIR}/nc/engine
                               ${ROOT_DIR}/nc/graphics
                               ${ROOT_DIR}/nc/graphics/d3dresource
                               ${ROOT_DIR}/nc/graphics/DirectXMath/Inc
                               ${ROOT_DIR}/nc/input
                               ${ROOT_DIR}/nc/math
                               ${ROOT_DIR}/nc/scene
                               ${ROOT_DIR}/nc/time
                               ${ROOT_DIR}/nc/utils/editor
                               ${ROOT_DIR}/nc/utils/editor/imgui
                               ${ROOT_DIR}/nc/utils/objloader
                               ${ROOT_DIR}/nc/views
                               ${ROOT_DIR}/nc/win32
                               ${ROOT_DIR}/project
                               ${ROOT_DIR}/project/components
                               ${ROOT_DIR}/project/models
                               ${ROOT_DIR}/project/scenes
                               ${ROOT_DIR}/project/settings
                          )

add_subdirectory(nc)

add_subdirectory(project)

#add_custom_command(TARGET ${EXE_NAME}
#                   POST_BUILD
#                     #copy project/models to build dir
#                    COMMAND ${CMAKE_COMMAND} -E copy_directory ${ROOT_DIR}/project/models/ ${PROJECT_BINARY_DIR}/project/models/
#
#                     #copy project/shaders to build dir
#                     COMMAND ${CMAKE_COMMAND} -E copy_directory ${ROOT_DIR}/project/shaders/ ${PROJECT_BINARY_DIR}/project/shaders/
#
#                     #copy project/settings to build dir
#                     COMMAND ${CMAKE_COMMAND} -E copy_directory ${ROOT_DIR}/project/settings/ ${PROJECT_BINARY_DIR}/project/settings/
#                  )