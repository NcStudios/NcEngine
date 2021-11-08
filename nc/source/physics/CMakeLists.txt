target_sources(${ENGINE_LIB}
    PRIVATE
        ${ROOT_DIR}/source/physics/ClickableSystem.cpp
        ${ROOT_DIR}/source/physics/Island.cpp
        ${ROOT_DIR}/source/physics/IslandBuilder.cpp
        ${ROOT_DIR}/source/physics/PhysicsSystemImpl.cpp
        ${ROOT_DIR}/source/physics/PhysicsWorld.cpp
)

add_subdirectory(collision)
add_subdirectory(dynamics)