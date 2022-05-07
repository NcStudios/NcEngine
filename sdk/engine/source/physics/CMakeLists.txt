target_sources(NcEngineLib
    PRIVATE
        ${PROJECT_SOURCE_DIR}/source/physics/ClickableSystem.cpp
        ${PROJECT_SOURCE_DIR}/source/physics/PhysicsPipelineTypes.cpp
        ${PROJECT_SOURCE_DIR}/source/physics/PhysicsModuleImpl.cpp
)

add_subdirectory(collision)
add_subdirectory(dynamics)