target_sources(${EXE_NAME} PUBLIC
                     #editor directory
                     ${ROOT_DIR}/nc/utils/editor/EditorManager.cpp
                     
                     #editor/imgui directory
                     ${ROOT_DIR}/nc/utils/editor/imgui/imgui_impl_dx11.cpp
                     ${ROOT_DIR}/nc/utils/editor/imgui/imgui_impl_win32.cpp
                     ${ROOT_DIR}/nc/utils/editor/imgui/imgui.cpp
                     ${ROOT_DIR}/nc/utils/editor/imgui/imgui_demo.cpp
                     ${ROOT_DIR}/nc/utils/editor/imgui/imgui_draw.cpp
                     ${ROOT_DIR}/nc/utils/editor/imgui/imgui_widgets.cpp
                     
                     #editor/objloader directory
                     ${ROOT_DIR}/nc/utils/objloader/ObjLoader.cpp)