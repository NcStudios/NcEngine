target_sources(${NC_ENGINE_LIB}
    PRIVATE
        CollisionQuery.cpp
        Constraints.cpp
        EventDispatch.cpp
        NcPhysicsImpl.cpp
        PhysicsUtility.cpp
        RigidBody.cpp
        Shape.cpp
)

add_subdirectory(jolt)
