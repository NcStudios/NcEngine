#include "ComponentFactories.h"
#include "ncengine/asset/DefaultAssets.h"
#include "ncengine/audio/AudioSource.h"
#include "ncengine/ecs/Logic.h"
#include "ncengine/ecs/ComponentRegistry.h"
#include "ncengine/graphics/ParticleEmitter.h"
#include "ncengine/graphics/PointLight.h"
#include "ncengine/graphics/MeshRenderer.h"
#include "ncengine/graphics/SkeletalAnimator.h"
#include "ncengine/graphics/SpotLight.h"
#include "ncengine/graphics/ToonRenderer.h"
#include "ncengine/network/NetworkDispatcher.h"
#include "ncengine/physics/Constraints.h"
#include "ncengine/physics/RigidBody.h"

namespace nc
{
auto CreateAudioSource(Entity entity, const std::any&) -> audio::AudioSource
{
    return audio::AudioSource{entity, {asset::DefaultAudioClip}};
}

auto CreateFrameLogic(Entity entity, const std::any&) -> FrameLogic
{
    return FrameLogic{entity, nullptr};
}

auto CreateParticleEmitter(Entity entity, const std::any&) -> graphics::ParticleEmitter
{
    return graphics::ParticleEmitter{entity, graphics::ParticleInfo{}};
}

auto CreatePointLight(Entity, const std::any&) -> graphics::PointLight
{
    return graphics::PointLight{};
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

auto CreateSpotLight(Entity, const std::any&) -> graphics::SpotLight
{
    return graphics::SpotLight{};
}

auto CreateNetworkDispatcher(Entity entity, const std::any&) -> net::NetworkDispatcher
{
    return net::NetworkDispatcher{entity};
}

auto CreateRigidBody(Entity entity, const std::any&) -> physics::RigidBody
{
    return physics::RigidBody{entity};
}
} // namespace nc
