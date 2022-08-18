set(IMGUI_SOURCE_DIR ${PROJECT_SOURCE_DIR}/source/external/imgui/src/)

add_library(imgui
    STATIC
        ${IMGUI_SOURCE_DIR}/imgui_impl_win32.cpp
        ${IMGUI_SOURCE_DIR}/imgui_impl_vulkan.cpp
        ${IMGUI_SOURCE_DIR}/imgui.cpp
        ${IMGUI_SOURCE_DIR}/imgui_draw.cpp
        ${IMGUI_SOURCE_DIR}/imgui_widgets.cpp
)

target_compile_definitions(imgui
    PUBLIC
        WIN32_LEAN_AND_MEAN
)

target_include_directories(imgui
    PRIVATE
        ${PROJECT_SOURCE_DIR}/source/external/
        ${PROJECT_SOURCE_DIR}/include
)

target_link_libraries(imgui
    PRIVATE
        Vulkan::Vulkan
)
