add_subdirectory(log)
add_subdirectory(ui)

target_sources(${EXE_NAME} 
    PRIVATE
    ${ROOT_DIR}/project/source/Prefabs.cpp
)