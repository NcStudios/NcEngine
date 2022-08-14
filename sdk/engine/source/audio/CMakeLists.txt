target_sources(${NC_ENGINE_LIB}
    PRIVATE
        AudioSource.cpp
        NcAudioImpl.cpp
)

add_subdirectory(rtaudio)