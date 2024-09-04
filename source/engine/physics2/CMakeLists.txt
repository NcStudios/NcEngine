target_sources(${NC_ENGINE_LIB}
    PRIVATE
        NcPhysicsImpl2.cpp
        RigidBody.cpp
        Shape.cpp
)

add_subdirectory(jolt)
