cmake_minimum_required(VERSION 3.10)
project("Sample" LANGUAGES CXX)

if(${PROJECT_SOURCE_DIR} EQUAL ${PROJECT_BINARY_DIR})
    message(FATAL_ERROR "Don't be a fool, out-of-source build with your tool.")
endif()

#Options
option(NC_TESTS_ENABLED "Include NcEngine tests" OFF)
option(USE_VULKAN "Use Vulkan rather than DirectX" OFF)
option(NC_EDITOR_ENABLED "Include in-game editor(also dear imgui)" OFF)
option(PROFILING_ENABLED "Include gprof instrumentation" OFF)
option(SANITIZE_UB "Use undefined behavior sanitizer" OFF)

#Variables
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(GAME_EXE "Sample")
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

set(LIB_DIR ${ROOT_DIR}/../nc/lib)
set(ENGINE_LIB "libNcEngine-${CMAKE_BUILD_TYPE}.a")
set(IMGUI_LIB "libimgui-${CMAKE_BUILD_TYPE}.a")
set(ENET_LIB "libenet-${CMAKE_BUILD_TYPE}.a")

add_executable(${GAME_EXE})

#Properties
set_target_properties(${GAME_EXE}
    PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${ROOT_DIR}/..
)

set_target_properties(${SERVER_EXE}
    PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${ROOT_DIR}/..
)

#Definitions
add_definitions(${WIN32_DEFINITIONS})

if(USE_VULKAN)
    add_definitions(-DUSE_VULKAN)
endif()

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
        ${NC_INCLUDE_DIR}
        ${PROJECT_INCLUDE_DIR}
        ${EXTERNAL_INCLUDE_DIR}
)

#Compile Options
target_compile_options(${GAME_EXE}
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

find_package(Vulkan REQUIRED)

#Link Options
target_link_libraries(${GAME_EXE}
    PRIVATE
        ${LIB_DIR}/${ENGINE_LIB}
        ${LIB_DIR}/${IMGUI_LIB}
        ${LIB_DIR}/windowscodecs.lib
        #${LIB_DIR}/${ENET_LIB} no networking sample yet
        ${WIN32_LINK_FLAGS}
)

if(USE_VULKAN)
    target_link_libraries(${GAME_EXE}
        PRIVATE
            Vulkan::Vulkan
            ${LIB_DIR}/glfw3.dll
            ${LIB_DIR}/libglfw3dll.a
            ${LIB_DIR}/libglfw3.a
    )
endif()

if(PROFILING_ENABLED)
    target_link_options(${GAME_EXE}
        PUBLIC
            -pg
    )
endif()

target_sources(${GAME_EXE}
    PRIVATE
        ${ROOT_DIR}/SampleMain.cpp
)

add_subdirectory(source/click_events)
add_subdirectory(source/collision_benchmark)
add_subdirectory(source/collision_events)
add_subdirectory(source/jare_scratch)
add_subdirectory(source/rendering_benchmark)
add_subdirectory(source/shared)
add_subdirectory(source/spawn_test)
add_subdirectory(source/worms)