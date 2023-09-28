# NcEngine CMake Root
add_library(${NC_ENGINE_LIB} STATIC)

# Propagate version
configure_file(${NC_SOURCE_DIR}/config/Version.h.in
               ${NC_INCLUDE_DIR}/ncengine/config/Version.h
)

# Definitions
target_compile_definitions(${NC_ENGINE_LIB}
    PUBLIC
        ${NC_COMPILE_DEFINITIONS}
        WIN32_LEAN_AND_MEAN
        $<INSTALL_INTERFACE:IMGUI_USER_CONFIG="$ncengine/ui/DearImGuiConversion.h">
        $<BUILD_INTERFACE:IMGUI_USER_CONFIG="${PROJECT_SOURCE_DIR}/include/ncengine/ui/DearImGuiConversion.h">
)

if(WIN32)
    target_compile_definitions(${NC_ENGINE_LIB} PRIVATE __WINDOWS_WASAPI__)
elseif(APPLE)
    target_compile_definitions(${NC_ENGINE_LIB} PRIVATE __MACOSX_CORE__)
endif()

if(NOT NC_PROD_BUILD)
    add_definitions(-DNC_ASSERT_ENABLED)
endif()

# Compile Options
target_compile_options(${NC_ENGINE_LIB}
    PRIVATE
        ${NC_COMPILER_FLAGS}
)

# Link libraries
target_link_libraries(${NC_ENGINE_LIB}
    INTERFACE
        imgui
    PUBLIC
        NcAsset
        NcMath
        NcUtility
        Vulkan::Vulkan
        Taskflow
        glfw
)

if(WIN32)
    target_link_libraries(${NC_ENGINE_LIB}
        INTERFACE
            gdi32
            ws2_32
            ole32
            winmm
            ksuser
            mfplat
            mfuuid
            wmcodecdspuuid
    )
elseif(APPLE)
    target_link_libraries(${NC_ENGINE_LIB}
        PUBLIC
            "-framework CoreAudio"
    )
endif()

if(${NC_PROFILING_ENABLED})
    target_link_libraries(${NC_ENGINE_LIB}
        PUBLIC
            OptickCore
    )
endif()

# Include Dirs
target_include_directories(${NC_ENGINE_LIB}
    PUBLIC
        $<INSTALL_INTERFACE:include/>
        $<INSTALL_INTERFACE:external/>
        $<BUILD_INTERFACE:${NC_INCLUDE_DIR}>
        $<BUILD_INTERFACE:${NC_EXTERNAL_DIR}>
    PRIVATE
        $<BUILD_INTERFACE:${NC_INCLUDE_DIR}/ncengine>
        $<BUILD_INTERFACE:${NC_SOURCE_DIR}>
)

# Install
install(TARGETS     ${NC_ENGINE_LIB} ${IMGUI_LIB} NcMath NcUtility NcAsset
        EXPORT      NcEngine-export
        DESTINATION lib
)
install(EXPORT      NcEngine-export
        FILE        NcEngineTargets.cmake
        NAMESPACE   Nc::
        DESTINATION lib/cmake/NcEngine
)
install(FILES       ${PROJECT_SOURCE_DIR}/NcEngineConfig.cmake
        DESTINATION lib/cmake/NcEngine
)
install(DIRECTORY   ${PROJECT_SOURCE_DIR}/include/ncengine
        DESTINATION include
)

add_subdirectory(audio)
add_subdirectory(assets)
add_subdirectory(config)
add_subdirectory(debug)
add_subdirectory(ecs)
add_subdirectory(engine)
add_subdirectory(graphics)
add_subdirectory(input)
add_subdirectory(module)
add_subdirectory(network)
add_subdirectory(particle)
add_subdirectory(physics)
add_subdirectory(scene)
add_subdirectory(task)
add_subdirectory(time)
add_subdirectory(ui)
add_subdirectory(utility)
add_subdirectory(window)
