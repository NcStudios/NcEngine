target_sources(${EXE_NAME} 
                 PUBLIC
                   #nc directory
                   ${ROOT_DIR}/nc/Entity.cpp
                   ${ROOT_DIR}/nc/NCE.cpp
                   ${ROOT_DIR}/nc/ProjectSettings.cpp)

add_subdirectory(component)
add_subdirectory(engine)
add_subdirectory(graphics)
add_subdirectory(input)
add_subdirectory(math)
add_subdirectory(scene)
add_subdirectory(time)
add_subdirectory(utils)
add_subdirectory(views)
add_subdirectory(win32)