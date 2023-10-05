#include "ComponentRegistration.h"
#include "audio/AudioSource.h"
#include "ecs/Logic.h"
#include "ecs/Registry.h"
#include "ecs/Transform.h"
#include "graphics/Camera.h"
#include "graphics/MeshRenderer.h"
#include "graphics/ParticleEmitter.h"
#include "graphics/PointLight.h"
#include "graphics/ToonRenderer.h"
#include "network/NetworkDispatcher.h"
#include "physics/Collider.h"
#include "physics/ConcaveCollider.h"
#include "physics/PhysicsBody.h"

#include "ui/editor/ComponentWidgets.h"

namespace nc
{
void RegisterEngineComponents(Registry& registry)
{
    registry.Handler<Transform>().drawUI = editor::TransformUIWidget;

    registry.RegisterComponentType<CollisionLogic>();
    registry.RegisterComponentType<FrameLogic>();
    registry.RegisterComponentType<FixedLogic>();
    registry.RegisterComponentType<audio::AudioSource>();
    registry.RegisterComponentType<graphics::MeshRenderer>();
    registry.RegisterComponentType<graphics::ParticleEmitter>();
    registry.RegisterComponentType<graphics::PointLight>();
    registry.RegisterComponentType<graphics::ToonRenderer>();
    registry.RegisterComponentType<net::NetworkDispatcher>();
    registry.RegisterComponentType<physics::Collider>();
    registry.RegisterComponentType<physics::ConcaveCollider>();
    registry.RegisterComponentType<physics::PhysicsBody>();
}
} // namespace nc
