target_sources(${GAME_EXE} 
    PRIVATE
        ${ROOT_DIR}/source/network/Packet.cpp
)

add_subdirectory(client)
add_subdirectory(server)