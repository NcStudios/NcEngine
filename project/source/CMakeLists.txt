add_subdirectory(log)
add_subdirectory(network)
add_subdirectory(ui)

target_sources(${GAME_EXE} 
    PRIVATE
    ${ROOT_DIR}/project/source/Prefabs.cpp
)