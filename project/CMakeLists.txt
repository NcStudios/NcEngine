file(GLOB USER_COMPONENT_SOURCE ${ROOT_DIR}/project/components/*.cpp)
file(GLOB USER_SCENE_SOURCE ${ROOT_DIR}/project/scenes/*.cpp)
file(GLOB USER_PROJECT_SOURCE ${ROOT_DIR}/project/source/*.cpp)
file(GLOB USER_UI_SOURCE ${ROOT_DIR}/project/ui/*.cpp)

target_sources(${EXE_NAME}
    PRIVATE
        ${ROOT_DIR}/project/WinMain.cpp
        ${USER_COMPONENT_SOURCE}
        ${USER_SCENE_SOURCE}
        ${USER_PROJECT_SOURCE}
        ${USER_UI_SOURCE}
)