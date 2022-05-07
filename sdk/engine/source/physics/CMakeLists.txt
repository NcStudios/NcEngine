target_sources(NcEngineLib
    PRIVATE
        ClickableSystem.cpp
        PhysicsPipelineTypes.cpp
        PhysicsModuleImpl.cpp
)

add_subdirectory(collision)
add_subdirectory(dynamics)