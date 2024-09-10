target_sources(${NC_ENGINE_LIB}
    PRIVATE
        EventDispatch.cpp
        NcPhysicsImpl2.cpp
        PhysicsUtility.cpp
        RigidBody.cpp
        Shape.cpp
)

add_subdirectory(jolt)
