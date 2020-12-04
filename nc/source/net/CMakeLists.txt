target_sources(${GAME_EXE} 
    PRIVATE
        ${ROOT_DIR}/nc/source/net/PacketBuffer.cpp
        ${ROOT_DIR}/nc/source/net/Network.cpp
)

target_sources(${SERVER_EXE}
    PRIVATE
        ${ROOT_DIR}/nc/source/net/PacketBuffer.cpp
)