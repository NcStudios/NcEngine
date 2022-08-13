target_sources(${NC_ENGINE_LIB}
    PRIVATE
        ClickableSystem.cpp
        Collider.cpp
        PhysicsBody.cpp
        PhysicsPipelineTypes.cpp
        PhysicsModuleImpl.cpp
)

add_subdirectory(collision)
add_subdirectory(dynamics)