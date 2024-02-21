#include "ComponentFactories.h"
#include "ncengine/asset/DefaultAssets.h"
#include "ncengine/audio/AudioSource.h"
#include "ncengine/ecs/Logic.h"
#include "ncengine/ecs/Registry.h"
#include "ncengine/graphics/ParticleEmitter.h"
#include "ncengine/graphics/PointLight.h"
#include "ncengine/graphics/MeshRenderer.h"
#include "ncengine/graphics/SkeletalAnimator.h"
#include "ncengine/graphics/ToonRenderer.h"
#include "ncengine/network/NetworkDispatcher.h"
#include "ncengine/physics/Collider.h"
#include "ncengine/physics/ConcaveCollider.h"
#include "ncengine/physics/PhysicsBody.h"

namespace nc
{
auto CreateAudioSource(Entity entity, void*) -> std::optional<audio::AudioSource>
{
    return audio::AudioSource{entity, {asset::DefaultAudioClip}};
}

auto CreateCollisionLogic(Entity entity, void*) -> std::optional<CollisionLogic>
{
    return CollisionLogic{entity, nullptr, nullptr, nullptr, nullptr};
}

auto CreateFixedLogic(Entity entity, void*) -> std::optional<FixedLogic>
{
    return FixedLogic{entity, nullptr};
}

auto CreateFrameLogic(Entity entity, void*) -> std::optional<FrameLogic>
{
    return FrameLogic{entity, nullptr};
}

auto CreateParticleEmitter(Entity entity, void*) -> std::optional<graphics::ParticleEmitter>
{
    return graphics::ParticleEmitter{entity, graphics::ParticleInfo{}};
}

auto CreatePointLight(Entity entity, void*) -> std::optional<graphics::PointLight>
{
    return graphics::PointLight{entity};
}

auto CreateMeshRenderer(Entity entity, void*) -> std::optional<graphics::MeshRenderer>
{
    return graphics::MeshRenderer{entity};
}

auto CreateToonRenderer(Entity entity, void*) -> std::optional<graphics::ToonRenderer>
{
    return graphics::ToonRenderer{entity};
}

auto CreateSkeletalAnimator(Entity entity, void*) -> std::optional<graphics::SkeletalAnimator>
{
    return graphics::SkeletalAnimator{entity, "dummyMesh", "dummyAnimation"};
}

auto CreateNetworkDispatcher(Entity entity, void*) -> std::optional<net::NetworkDispatcher>
{
    return net::NetworkDispatcher{entity};
}

auto CreateCollider(Entity entity, void*) -> std::optional<physics::Collider>
{
    return physics::Collider{entity, physics::BoxProperties{}};
}

auto CreateConcaveCollider(Entity entity, void*) -> std::optional<physics::ConcaveCollider>
{
    return entity.IsStatic()
        ? std::optional<physics::ConcaveCollider>{physics::ConcaveCollider{entity, asset::DefaultConcaveCollider}}
        : std::optional<physics::ConcaveCollider>{std::nullopt};
}

auto CreatePhysicsBody(Entity entity, void* userData) -> std::optional<physics::PhysicsBody>
{
    NC_ASSERT(userData, "Expected non-null user data.");
    auto colliderPool = static_cast<ecs::ComponentPool<physics::Collider>*>(userData);
    if (!colliderPool->Contains(entity))
    {
        colliderPool->Emplace(entity, physics::BoxProperties{});
    }

    return physics::PhysicsBody{entity};
}
} // namespace nc
