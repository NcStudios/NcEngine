# NcEngine CMake Root
add_library(${NC_ENGINE_LIB} STATIC)

# Propagate version
configure_file(${NC_SOURCE_DIR}/config/Version.h.in
               ${NC_INCLUDE_DIR}/config/Version.h
)

# Definitions
target_compile_definitions(${NC_ENGINE_LIB}
    PUBLIC
        WIN32_LEAN_AND_MEAN
        $<INSTALL_INTERFACE:IMGUI_USER_CONFIG="$ui/DearImGuiConversion.h">
        $<BUILD_INTERFACE:IMGUI_USER_CONFIG="${PROJECT_SOURCE_DIR}/sdk/engine/include/ui/DearImGuiConversion.h">
    PRIVATE
        __WINDOWS_WASAPI__
)

if(NOT NC_PROD_BUILD)
    add_definitions(-DNC_ASSERT_ENABLED)
    target_compile_definitions(${NC_ENGINE_LIB}
        PUBLIC
            ${NC_COMPILE_DEFINITIONS}
    )
endif()

# Compile Options
target_compile_options(${NC_ENGINE_LIB}
    PRIVATE
        ${WARNING_FLAGS}
)

# Link libraries
target_link_libraries(${NC_ENGINE_LIB}
    INTERFACE
        imgui
        gdi32
        ws2_32
        ole32
        winmm
        ksuser
        mfplat
        mfuuid
        wmcodecdspuuid
        #enet add for project requiring networking
    PUBLIC
        Vulkan::Vulkan
)

# Note - OptickCore.dll still needs to be copied to the executable's directory
if(${NC_PROFILING_ENABLED})
    target_link_libraries(${NC_ENGINE_LIB}
        INTERFACE
            $<INSTALL_INTERFACE:${NC_ENGINE_INSTALL_DIR}/lib/OptickCore.dll>
    )
endif()

# Include Dirs
target_include_directories(${NC_ENGINE_LIB}
    PUBLIC
        $<INSTALL_INTERFACE:include/>
        $<INSTALL_INTERFACE:external/>
    PRIVATE
        $<BUILD_INTERFACE:${NC_INCLUDE_DIR}>
        $<BUILD_INTERFACE:${NC_SOURCE_DIR}>
        $<BUILD_INTERFACE:${NC_EXTERNAL_DIR}>
)

add_subdirectory(source)
