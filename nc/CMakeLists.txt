cmake_minimum_required(VERSION 3.10)
project("NC Engine" VERSION "0.1.0" LANGUAGES CXX C)

if(${PROJECT_SOURCE_DIR} EQUAL ${PROJECT_BINARY_DIR})
    message(FATAL_ERROR "Don't be a fool, out-of-source build with your tool.")
endif()

#OPTIONS
option(NC_TESTS_ENABLED "Include NcEngine tests" OFF)
option(NC_EDITOR_ENABLED "Include in-game editor(also dear imgui)" OFF)
option(PROFILING_ENABLED "Include gprof instrumentation" OFF)
option(SANITIZE_UB "Use undefined behavior santizer" OFF)

#VARIABLES
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(ROOT_DIR "${PROJECT_SOURCE_DIR}")
set(LIB_DIR "${ROOT_DIR}/lib")
set(EXTERNAL_INCLUDE_DIR "${ROOT_DIR}/external/include")
set(EXTERNAL_SOURCE_DIR "${ROOT_DIR}/source")
set(NC_INCLUDE_DIR "${ROOT_DIR}/include")
set(NC_SOURCE_DIR "${ROOT_DIR}/source")

set(GCC_WARNING_FLAGS 
    -Wall
    -Wextra
    -Wfatal-errors
    -Wno-cast-function-type
)
set(WIN32_LINK_FLAGS "-lgdi32 -ld3d11 -ld3dcompiler_47 -lwinmm -lws2_32")
set(WIN32_DEFINITIONS -DWIN32_LEAN_AND_MEAN)

set(ENGINE_LIB "NcEngine-${CMAKE_BUILD_TYPE}")
set(IMGUI_LIB "imgui-${CMAKE_BUILD_TYPE}")
set(ENET_LIB "enet-${CMAKE_BUILD_TYPE}")

#Propogate version
configure_file(${ROOT_DIR}/source/config/Version.h.in
               ${ROOT_DIR}/source/config/Version.h
)

#Definitions
add_definitions(${WIN32_DEFINITIONS})

if(NC_EDITOR_ENABLED)
    add_definitions(-DNC_EDITOR_ENABLED)
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_definitions(-DNC_DEBUG_BUILD)
endif()

if(VERBOSE_LOGGING_ENABLED)
    add_definitions(-DVERBOSE_LOGGING_ENABLED)
endif()

### TARGETS ###
add_library(${ENGINE_LIB} STATIC)

#Properties
set_target_properties(${ENGINE_LIB}
    PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY ${LIB_DIR}
)

#Compile Options
target_compile_options(${ENGINE_LIB}
    PRIVATE
        ${GCC_WARNING_FLAGS}
)

if(SANITIZE_UB)
    target_compile_options(${ENGINE_LIB}
        PRIVATE
            -fsanitize=undefined
            -fsanitize-undefined-trap-on-error
    )
endif()

if(PROFILING_ENABLED)
    target_compile_options(${ENGINE_LIB}
        PRIVATE
            -pg
    )
endif()

#Include Dirs
target_include_directories(${ENGINE_LIB}
    PRIVATE
        ${NC_INCLUDE_DIR}
        ${NC_SOURCE_DIR}
        ${EXTERNAL_INCLUDE_DIR}
)

#Link Options
if(PROFILING_ENABLED)
    target_link_options(${ENGINE_LIB}
        PRIVATE
            -pg
    )
endif()

#SUBDIRS
add_subdirectory(source)
add_subdirectory(external/src/enet)
add_subdirectory(external/src/imgui)

if(NC_TESTS_ENABLED)
    enable_testing()
    add_subdirectory(external/src/gtest)
    add_subdirectory(test)
endif()