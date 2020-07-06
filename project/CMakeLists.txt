file(GLOB USER_COMPONENT_SOURCE ${ROOT_DIR}/project/components/*.cpp)
file(GLOB USER_SCENE_SOURCE ${ROOT_DIR}/project/scenes/*.cpp)

target_sources(${EXE_NAME}
    PRIVATE
        ${USER_COMPONENT_SOURCE}
        ${USER_SCENE_SOURCE}
)