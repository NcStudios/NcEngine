target_sources(NcEngineLib
    PRIVATE
        ${PROJECT_SOURCE_DIR}/source/physics/collision/BspTree.cpp
        ${PROJECT_SOURCE_DIR}/source/physics/collision/IntersectionQueries.cpp
)

add_subdirectory(broad_phase)
add_subdirectory(narrow_phase)