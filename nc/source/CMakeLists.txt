target_sources(${EXE_NAME} 
    PRIVATE
        ${ROOT_DIR}/nc/source/entity/Entity.cpp
)

add_subdirectory(component)
add_subdirectory(config)
add_subdirectory(engine)
add_subdirectory(graphics)
add_subdirectory(input)
add_subdirectory(math)
add_subdirectory(time)
add_subdirectory(utils)
add_subdirectory(win32)