add_subdirectory(external/imgui/src)
add_subdirectory(external/lz4)
add_subdirectory(external/rtaudio)
add_subdirectory(ncutility)
add_subdirectory(ncmath)
add_subdirectory(ncasset)

if(NC_BUILD_NCCONVERT)
    add_subdirectory(ncconvert)
endif()

add_subdirectory(ncengine)
