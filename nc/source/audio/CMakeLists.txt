target_sources(${ENGINE_LIB}
    PRIVATE
        ${ROOT_DIR}/source/audio/AudioSystemImpl.cpp
)

add_subdirectory(rtaudio)