#include "ComponentFactories.h"
#include "ncengine/asset/DefaultAssets.h"
#include "ncengine/audio/AudioSource.h"
#include "ncengine/ecs/Logic.h"
#include "ncengine/ecs/ComponentRegistry.h"
#include "ncengine/graphics/ParticleEmitter.h"
#include "ncengine/graphics/PointLight.h"
#include "ncengine/graphics/MeshRenderer.h"
#include "ncengine/graphics/SkeletalAnimator.h"
#include "ncengine/graphics/ToonRenderer.h"
#include "ncengine/network/NetworkDispatcher.h"
#include "ncengine/physics/Collider.h"
#include "ncengine/physics/Constraints.h"
#include "ncengine/physics/PhysicsBody.h"
#include "ncengine/physics/PhysicsMaterial.h"

namespace nc
{
auto CreateAudioSource(Entity entity, const std::any&) -> audio::AudioSource
{
    return audio::AudioSource{entity, {asset::DefaultAudioClip}};
}

auto CreateCollisionLogic(Entity entity, const std::any&) -> CollisionLogic
{
    return CollisionLogic{entity, nullptr, nullptr, nullptr, nullptr};
}

auto CreateFixedLogic(Entity entity, const std::any&) -> FixedLogic
{
    return FixedLogic{entity, nullptr};
}

auto CreateFrameLogic(Entity entity, const std::any&) -> FrameLogic
{
    return FrameLogic{entity, nullptr};
}

auto CreateParticleEmitter(Entity entity, const std::any&) -> graphics::ParticleEmitter
{
    return graphics::ParticleEmitter{entity, graphics::ParticleInfo{}};
}

auto CreatePointLight(Entity entity, const std::any&) -> graphics::PointLight
{
    return graphics::PointLight{entity};
}

auto CreateMeshRenderer(Entity entity, const std::any&) -> graphics::MeshRenderer
{
    return graphics::MeshRenderer{entity};
}

auto CreateToonRenderer(Entity entity, const std::any&) -> graphics::ToonRenderer
{
    return graphics::ToonRenderer{entity};
}

auto CreateSkeletalAnimator(Entity entity, const std::any&) -> graphics::SkeletalAnimator
{
    return graphics::SkeletalAnimator{entity, "dummyMesh", "dummyAnimation"};
}

auto CreateNetworkDispatcher(Entity entity, const std::any&) -> net::NetworkDispatcher
{
    return net::NetworkDispatcher{entity};
}

auto CreateCollider(Entity entity, const std::any&) -> physics::Collider
{
    return physics::Collider{entity, physics::BoxProperties{}};
}

auto CreatePhysicsBody(Entity entity, const std::any& userData) -> physics::PhysicsBody
{
    auto registry = std::any_cast<ecs::ComponentRegistry*>(userData);
    auto& transform = registry->GetPool<Transform>().Get(entity);
    auto& colliderPool = registry->GetPool<physics::Collider>();
    auto& collider = colliderPool.Contains(entity)
        ? colliderPool.Get(entity)
        : colliderPool.Emplace(entity, physics::BoxProperties{});

    return physics::PhysicsBody{transform, collider};
}

auto CreatePhysicsMaterial(Entity, const std::any&) -> physics::PhysicsMaterial
{
    return physics::PhysicsMaterial{};
}

auto CreatePositionClamp(Entity, const std::any&) -> physics::PositionClamp
{
    return physics::PositionClamp{};
}

auto CreateVelocityRestriction(Entity, const std::any&) -> physics::VelocityRestriction
{
    return physics::VelocityRestriction{};
}
} // namespace nc
