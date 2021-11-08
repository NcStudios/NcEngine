target_sources(${ENGINE_LIB}
    PRIVATE
        ${ROOT_DIR}/source/physics/collision/BspTree.cpp
        ${ROOT_DIR}/source/physics/collision/IntersectionQueries.cpp
)

add_subdirectory(narrow_phase)
add_subdirectory(proxy)