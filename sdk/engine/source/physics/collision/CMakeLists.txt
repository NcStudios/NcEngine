target_sources(NcEngineLib
    PRIVATE
        BspTree.cpp
        IntersectionQueries.cpp
)

add_subdirectory(broad_phase)
add_subdirectory(narrow_phase)