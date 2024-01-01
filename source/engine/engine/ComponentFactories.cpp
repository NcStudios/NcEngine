#include "ComponentFactories.h"
#include "ncengine/asset/DefaultAssets.h"
#include "ncengine/audio/AudioSource.h"
#include "ncengine/ecs/Logic.h"
#include "ncengine/ecs/Registry.h"
#include "ncengine/graphics/ParticleEmitter.h"
#include "ncengine/graphics/MeshRenderer.h"
#include "ncengine/graphics/SkeletalAnimator.h"
#include "ncengine/graphics/ToonRenderer.h"
#include "ncengine/network/NetworkDispatcher.h"
#include "ncengine/physics/Collider.h"
#include "ncengine/physics/PhysicsBody.h"

namespace nc
{
auto CreateAudioSource(Entity entity, void*) -> audio::AudioSource
{
    return audio::AudioSource{entity, asset::DefaultAudioClip};
}

auto CreateCollisionLogic(Entity entity, void*) -> CollisionLogic
{
    return CollisionLogic{entity, nullptr, nullptr, nullptr, nullptr};
}

auto CreateFixedLogic(Entity entity, void*) -> FixedLogic
{
    return FixedLogic{entity, nullptr};
}

auto CreateFrameLogic(Entity entity, void*) -> FrameLogic
{
    return FrameLogic{entity, nullptr};
}

auto CreateParticleEmitter(Entity entity, void*) -> graphics::ParticleEmitter
{
    return graphics::ParticleEmitter{entity, graphics::ParticleInfo{}};
}

auto CreateMeshRenderer(Entity entity, void*) -> graphics::MeshRenderer
{
    return graphics::MeshRenderer{entity};
}

auto CreateToonRenderer(Entity entity, void*) -> graphics::ToonRenderer
{
    return graphics::ToonRenderer{entity};
}

auto CreateSkeletalAnimator(Entity entity, void*) -> graphics::SkeletalAnimator
{
    return graphics::SkeletalAnimator{entity, "@todo"};
}

auto CreateNetworkDispatcher(Entity entity, void*) -> net::NetworkDispatcher
{
    return net::NetworkDispatcher{entity};
}

auto CreateCollider(Entity entity, void*) -> physics::Collider
{
    return physics::Collider{entity, physics::BoxProperties{}};
}

auto CreatePhysicsBody(Entity entity, void* userData) -> physics::PhysicsBody
{
    NC_ASSERT(userData, "Expected non-null user data.");
    auto registry = static_cast<Registry*>(userData);
    if (!registry->Contains<physics::Collider>(entity))
    {
        registry->Add<physics::Collider>(entity, physics::BoxProperties{});
    }

    return physics::PhysicsBody{entity};
}
} // namespace nc
