target_sources(${EXE_NAME}
    PRIVATE
        #component source files
        ${ROOT_DIR}/project/components/CamController.cpp
        ${ROOT_DIR}/project/components/Head.cpp
        ${ROOT_DIR}/project/components/Timer.cpp

        #scene source files
        ${ROOT_DIR}/project/scenes/InitialScene.cpp
)