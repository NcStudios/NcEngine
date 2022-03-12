target_sources(NcEngineLib
    PRIVATE
        ${ROOT_DIR}/source/physics/ClickableSystem.cpp
        ${ROOT_DIR}/source/physics/PhysicsPipelineTypes.cpp
        ${ROOT_DIR}/source/physics/PhysicsModuleImpl.cpp
)

add_subdirectory(collision)
add_subdirectory(dynamics)