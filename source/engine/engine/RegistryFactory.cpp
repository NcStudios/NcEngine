#include "RegistryFactories.h"
#include "ComponentFactories.h"
#include "ncengine/audio/AudioSource.h"
#include "ncengine/ecs/Logic.h"
#include "ncengine/ecs/Registry.h"
#include "ncengine/ecs/Tag.h"
#include "ncengine/ecs/Transform.h"
#include "ncengine/ecs/detail/FreeComponentGroup.h"
#include "ncengine/graphics/Camera.h"
#include "ncengine/graphics/MeshRenderer.h"
#include "ncengine/graphics/ParticleEmitter.h"
#include "ncengine/graphics/PointLight.h"
#include "ncengine/graphics/ToonRenderer.h"
#include "ncengine/graphics/SkeletalAnimator.h"
#include "ncengine/network/NetworkDispatcher.h"
#include "ncengine/physics/Collider.h"
#include "ncengine/physics/ConcaveCollider.h"
#include "ncengine/physics/PhysicsBody.h"
#include "ncengine/type/ComponentId.h"
#include "ncengine/utility/Log.h"
#include "ui/editor/ComponentWidgets.h"

namespace
{
template<class T>
void Register(nc::Registry& registry,
              size_t id,
              const char* name,
              typename nc::ComponentHandler<T>::DrawUI_t&& drawUI = nullptr,
              typename nc::ComponentHandler<T>::Factory_t&& factory = nullptr,
              typename nc::ComponentHandler<T>::Serialize_t&& serialize = nullptr,
              typename nc::ComponentHandler<T>::Deserialize_t&& deserialize = nullptr,
              void* userData = nullptr)
{
    registry.RegisterComponentType<T>(nc::ComponentHandler<T>
    {
        .id = id,
        .name = name,
        .userData = userData,
        .factory = std::move(factory),
        .serialize = std::move(serialize),
        .deserialize = std::move(deserialize),
        .drawUI = std::move(drawUI)
    });
}
} // anonymous namespace

namespace nc
{
auto BuildRegistry(size_t maxEntities) -> std::unique_ptr<Registry>
{
    NC_LOG_INFO("Building registry");
    auto registry = std::make_unique<Registry>(maxEntities);
    Register<ecs::detail::FreeComponentGroup>(*registry, FreeComponentGroupId, "");
    Register<Tag>(*registry, TagId, "Tag", editor::TagUIWidget);
    Register<Transform>(*registry, TransformId, "Transform", editor::TransformUIWidget);
    Register<graphics::MeshRenderer>(*registry, MeshRendererId, "MeshRenderer", editor::MeshRendererUIWidget, CreateMeshRenderer);
    Register<graphics::ToonRenderer>(*registry, ToonRendererId, "ToonRenderer", editor::ToonRendererUIWidget, CreateToonRenderer);
    Register<graphics::SkeletalAnimator>(*registry, SkeletalAnimatorId, "SkeletalAnimator", editor::SkeletalAnimatorUIWidget, CreateSkeletalAnimator);
    Register<graphics::PointLight>(*registry, PointLightId, "PointLight", editor::PointLightUIWidget);
    Register<graphics::ParticleEmitter>(*registry, ParticleEmitterId, "ParticleEmitter", editor::ParticleEmitterUIWidget, CreateParticleEmitter);
    Register<physics::Collider>(*registry, ColliderId, "Collider", editor::ColliderUIWidget, CreateCollider);
    Register<physics::ConcaveCollider>(*registry, ConcaveColliderId, "ConcaveCollider", editor::ConcaveColliderUIWidget);
    Register<physics::PhysicsBody>(*registry, PhysicsBodyId, "PhysicsBody", editor::PhysicsBodyUIWidget, CreatePhysicsBody, nullptr, nullptr, static_cast<void*>(registry.get()));
    Register<FrameLogic>(*registry, FrameLogicId, "FrameLogic", editor::FrameLogicUIWidget, CreateFrameLogic);
    Register<FixedLogic>(*registry, FixedLogicId, "FixedLogic", editor::FixedLogicUIWidget, CreateFixedLogic);
    Register<CollisionLogic>(*registry, CollisionLogicId, "CollisionLogic", editor::CollisionLogicUIWidget, CreateCollisionLogic);
    Register<audio::AudioSource>(*registry, AudioSourceId, "AudioSource", editor::AudioSourceUIWidget, CreateAudioSource);
    Register<net::NetworkDispatcher>(*registry, NetworkDispatcherId, "NetworkDispatcher", editor::NetworkDispatcherUIWidget, CreateNetworkDispatcher);
    return registry;
}
} // namespace nc
