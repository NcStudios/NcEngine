target_sources(${NC_ENGINE_LIB}
    PRIVATE
        AudioModuleImpl.cpp
        AudioSource.cpp
)

add_subdirectory(rtaudio)