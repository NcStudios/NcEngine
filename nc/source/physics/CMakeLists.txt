target_sources(${ENGINE_LIB}
    PRIVATE
        ${ROOT_DIR}/source/physics/ClickableSystem.cpp
        ${ROOT_DIR}/source/physics/PhysicsPipelineTypes.cpp
        ${ROOT_DIR}/source/physics/PhysicsSystemImpl.cpp
)

add_subdirectory(collision)
add_subdirectory(dynamics)