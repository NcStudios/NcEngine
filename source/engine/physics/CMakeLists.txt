target_sources(${NC_ENGINE_LIB}
    PRIVATE
        ClickableSystem.cpp
        Collider.cpp
        ConcaveCollider.cpp
        NcPhysicsImpl.cpp
        PhysicsBody.cpp
        PhysicsPipelineTypes.cpp
)

add_subdirectory(collision)
add_subdirectory(dynamics)