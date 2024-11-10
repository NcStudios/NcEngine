target_sources(${NC_ENGINE_LIB}
    PRIVATE
        Material.cpp
        NcGraphicsImpl2.cpp
)

add_subdirectory(diligent)
add_subdirectory(frontend)
