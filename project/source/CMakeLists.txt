add_subdirectory(log)
add_subdirectory(ui)

target_sources(${GAME_EXE} 
    PRIVATE
    ${ROOT_DIR}/source/Prefabs.cpp
)