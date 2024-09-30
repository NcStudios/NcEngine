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

if(NC_PHYSICS_CONFIG)
    message("Using user-provided physics config: ${NC_PHYSICS_CONFIG}")
    add_definitions(-DNC_PHYSICS_CONFIG="${NC_PHYSICS_CONFIG}")
endif()

# Add debug post fix of 'd' to NC_ENGINE_LIB targets
set_target_properties(${NC_ENGINE_LIB}
    PROPERTIES CMAKE_DEBUG_POSTFIX d
)

SET(DILIGENT_LIBRARIES
    Diligent-TargetPlatform
    Diligent-GraphicsEngineOpenGL-shared
    Diligent-GraphicsEngineVk-shared
)

if(WIN32)
    list(APPEND DILIGENT_LIBRARIES 
        Diligent-GraphicsEngineD3D11-shared 
        Diligent-GraphicsEngineD3D12-shared
    )
endif()

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
        Jolt
        ${DILIGENT_LIBRARIES}
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
        $<BUILD_INTERFACE:${diligentcore_SOURCE_DIR}>
)

target_include_directories(${NC_ENGINE_LIB}
    SYSTEM PUBLIC
        $<INSTALL_INTERFACE:external/>
        $<BUILD_INTERFACE:${NC_EXTERNAL_DIR}>
        $<BUILD_INTERFACE:${VMA_INCLUDE_DIR}>
)

add_subdirectory(audio)
add_subdirectory(asset)
add_subdirectory(config)
add_subdirectory(debug)
add_subdirectory(ecs)
add_subdirectory(engine)
add_subdirectory(graphics)
add_subdirectory(graphics2)
add_subdirectory(input)
add_subdirectory(network)
add_subdirectory(particle)
add_subdirectory(physics)
add_subdirectory(physics2)
add_subdirectory(serialize)
add_subdirectory(scene)
add_subdirectory(task)
add_subdirectory(time)
add_subdirectory(ui)
add_subdirectory(utility)
add_subdirectory(window)
