set(IMGUI_SOURCE_DIR "${ROOT_DIR}/external/imgui/src/")

add_library(${IMGUI_LIB}
    STATIC
        ${IMGUI_SOURCE_DIR}/imgui_impl_win32.cpp
        ${IMGUI_SOURCE_DIR}/imgui_impl_vulkan.cpp
        ${IMGUI_SOURCE_DIR}/imgui.cpp
        ${IMGUI_SOURCE_DIR}/imgui_draw.cpp
        ${IMGUI_SOURCE_DIR}/imgui_widgets.cpp
)

find_package(Vulkan REQUIRED)

target_compile_definitions(${IMGUI_LIB}
    PUBLIC
        WIN32_LEAN_AND_MEAN
)

target_include_directories(${IMGUI_LIB}
    PRIVATE
        ${NC_INCLUDE_DIR}
        ${NC_EXTERNAL_DIR}
        ${NC_EXTERNAL_DIR}
)

target_link_libraries(${IMGUI_LIB}
    PRIVATE
        Vulkan::Vulkan
)

set_target_properties(${IMGUI_LIB}
    PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY $<BUILD_INTERFACE:${LIB_DIR}>
)