target_sources(${NC_ENGINE_LIB}
    PRIVATE
        NcPhysicsImpl2.cpp
        RigidBody.cpp
)

add_subdirectory(jolt)
