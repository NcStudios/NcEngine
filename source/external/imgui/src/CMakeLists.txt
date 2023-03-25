set(IMGUI_SOURCE_DIR ${PROJECT_SOURCE_DIR}/source/external/imgui/src/)

add_library(imgui
    STATIC
        ${IMGUI_SOURCE_DIR}/imgui_impl_glfw.cpp
        
        ${IMGUI_SOURCE_DIR}/imgui_impl_vulkan.cpp
        ${IMGUI_SOURCE_DIR}/imgui.cpp
        ${IMGUI_SOURCE_DIR}/imgui_draw.cpp
        ${IMGUI_SOURCE_DIR}/imgui_widgets.cpp
)

target_compile_definitions(imgui
    PUBLIC
        WIN32_LEAN_AND_MEAN
)

set(NC_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/include")
set(NC_SOURCE_DIR "${PROJECT_SOURCE_DIR}/source/engine/")
set(NC_EXTERNAL_DIR "${PROJECT_SOURCE_DIR}/source/external")

# Include Dirs
target_include_directories(imgui
    PUBLIC
        $<INSTALL_INTERFACE:include/>
        $<INSTALL_INTERFACE:external/>
        $<BUILD_INTERFACE:${NC_INCLUDE_DIR}>
        $<BUILD_INTERFACE:${NC_EXTERNAL_DIR}>
    PRIVATE
        ${PROJECT_SOURCE_DIR}/source/external/
        ${PROJECT_SOURCE_DIR}/include
        $<BUILD_INTERFACE:${NC_INCLUDE_DIR}/ncengine>
        $<BUILD_INTERFACE:${NC_SOURCE_DIR}>
)

target_link_libraries(imgui
    PRIVATE
        Vulkan::Vulkan
    PUBLIC
        glfw

)
