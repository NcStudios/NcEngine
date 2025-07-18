add_library(imguizmo STATIC
    ${imguizmo_SOURCE_DIR}/ImGuizmo.cpp
)

target_compile_definitions(imguizmo
    PUBLIC
        WIN32_LEAN_AND_MEAN
)

target_include_directories(imguizmo
    PUBLIC
        "${imguizmo_SOURCE_DIR}"
)

target_link_libraries(imguizmo
    PRIVATE
        imgui
)
