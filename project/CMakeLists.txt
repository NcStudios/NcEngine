file(GLOB USER_COMPONENT_SOURCE ${ROOT_DIR}/project/components/*.cpp)
file(GLOB USER_SCENE_SOURCE ${ROOT_DIR}/project/scenes/*.cpp)
file(GLOB USER_UI_SOURCE ${ROOT_DIR}/project/ui/*.cpp)

target_sources(${GAME_EXE}
    PRIVATE
        ${ROOT_DIR}/project/CavernaMain.cpp
        ${USER_COMPONENT_SOURCE}
        ${USER_SCENE_SOURCE}
        ${USER_UI_SOURCE}
)

target_sources(${SERVER_EXE}
    PRIVATE
        ${ROOT_DIR}/project/ServerMain.cpp)

add_subdirectory(config)
add_subdirectory(source)