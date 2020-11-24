target_sources(${GAME_EXE} 
    PRIVATE
        ${ROOT_DIR}/project/source/network/Packet.cpp
)

target_sources(${SERVER_EXE}
    PRIVATE
        ${ROOT_DIR}/project/source/network/Packet.cpp
)

add_subdirectory(client)
add_subdirectory(server)