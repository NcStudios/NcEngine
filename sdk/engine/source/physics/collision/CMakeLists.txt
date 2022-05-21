target_sources(${NC_ENGINE_LIB}
    PRIVATE
        BspTree.cpp
        IntersectionQueries.cpp
)

add_subdirectory(broad_phase)
add_subdirectory(narrow_phase)