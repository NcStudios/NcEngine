add_subdirectory(external/imgui/src)
add_subdirectory(external/rtaudio)
add_subdirectory(ncasset)

if(NC_BUILD_NCCONVERT)
    add_subdirectory(ncconvert)
endif()

add_subdirectory(engine)
