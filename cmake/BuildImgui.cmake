add_library(imgui STATIC
    ${imgui_SOURCE_DIR}/imgui.cpp
    ${imgui_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_SOURCE_DIR}/imgui_tables.cpp
    ${imgui_SOURCE_DIR}/imgui_widgets.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
)

if(WIN32)
    target_sources(imgui
        PRIVATE
            ${imgui_SOURCE_DIR}/backends/imgui_impl_win32.cpp
    )
endif()

target_compile_definitions(imgui
    PUBLIC
        WIN32_LEAN_AND_MEAN
        ImTextureID=void*
)

target_include_directories(imgui
    PUBLIC
        "${imgui_SOURCE_DIR}"
)

target_link_libraries(imgui
    PRIVATE
        Vulkan::Vulkan
        glfw
)
