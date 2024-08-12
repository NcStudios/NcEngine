# NcEngine CMake Root
add_library(${NC_ENGINE_LIB} STATIC)

# Propagate version
configure_file(${PROJECT_SOURCE_DIR}/cmake/Version.h.in
               ${NC_INCLUDE_DIR}/ncengine/config/Version.h
)

# Definitions
target_compile_definitions(${NC_ENGINE_LIB}
    PUBLIC
        ${NC_COMPILE_DEFINITIONS}
        WIN32_LEAN_AND_MEAN
        $<INSTALL_INTERFACE:IMGUI_USER_CONFIG="$ncengine/ui/ImGuiConversion.h">
        $<BUILD_INTERFACE:IMGUI_USER_CONFIG="${PROJECT_SOURCE_DIR}/include/ncengine/ui/ImGuiConversion.h">
)

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
        OptickCore
        RtAudio
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

# Include Dirs
target_include_directories(${NC_ENGINE_LIB}
    PUBLIC
        $<INSTALL_INTERFACE:include/>
        $<BUILD_INTERFACE:${NC_INCLUDE_DIR}>
    PRIVATE
        $<BUILD_INTERFACE:${NC_INCLUDE_DIR}/ncengine>
        $<BUILD_INTERFACE:${NC_SOURCE_DIR}>
)

target_include_directories(${NC_ENGINE_LIB}
    SYSTEM PUBLIC
        $<INSTALL_INTERFACE:external/>
        $<BUILD_INTERFACE:${NC_EXTERNAL_DIR}>
        $<BUILD_INTERFACE:${VMA_INCLUDE_DIR}>
)

# Install
install(TARGETS     ${NC_ENGINE_LIB} imgui NcMath NcUtility NcAsset RtAudio
        EXPORT      NcEngine-export
        DESTINATION lib
)
install(EXPORT      NcEngine-export
        FILE        NcEngineTargets.cmake
        NAMESPACE   Nc::
        DESTINATION lib/cmake/NcEngine
)
install(FILES       ${PROJECT_SOURCE_DIR}/cmake/NcEngineConfig.cmake
        DESTINATION lib/cmake/NcEngine
)
install(DIRECTORY   ${PROJECT_SOURCE_DIR}/include/ncengine
        DESTINATION include
)

add_subdirectory(audio)
add_subdirectory(asset)
add_subdirectory(config)
add_subdirectory(debug)
add_subdirectory(ecs)
add_subdirectory(engine)
add_subdirectory(graphics)
add_subdirectory(input)
add_subdirectory(network)
add_subdirectory(particle)
add_subdirectory(physics)
add_subdirectory(serialize)
add_subdirectory(scene)
add_subdirectory(task)
add_subdirectory(time)
add_subdirectory(ui)
add_subdirectory(utility)
add_subdirectory(window)
