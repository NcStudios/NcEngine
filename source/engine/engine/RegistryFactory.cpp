#include "RegistryFactory.h"
#include "ComponentFactories.h"
#include "ncengine/audio/AudioSource.h"
#include "ncengine/ecs/ComponentRegistry.h"
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
#include "ncengine/graphics/WireframeRenderer.h"
#include "ncengine/network/NetworkDispatcher.h"
#include "ncengine/physics/Collider.h"
#include "ncengine/physics/ConcaveCollider.h"
#include "ncengine/physics/PhysicsBody.h"
#include "ncengine/type/EngineId.h"
#include "ncengine/utility/Log.h"
#include "serialize/ComponentSerialization.h"
#include "ui/editor/ComponentWidgets.h"

namespace
{
template<class T>
void Register(nc::ecs::ComponentRegistry& registry,
              size_t maxEntities, // could add specific values for each type again
              size_t id,
              const char* name,
              typename nc::ComponentHandler<T>::DrawUI_t&& drawUI = nullptr,
              typename nc::ComponentHandler<T>::Factory_t&& factory = nullptr,
              typename nc::ComponentHandler<T>::Serialize_t&& serialize = nullptr,
              typename nc::ComponentHandler<T>::Deserialize_t&& deserialize = nullptr,
              void* userData = nullptr)
{
    registry.RegisterType<T>(maxEntities, nc::ComponentHandler<T>
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
auto BuildRegistry(size_t maxEntities) -> std::unique_ptr<ecs::ComponentRegistry>
{
    NC_LOG_INFO("Building registry");
    auto registry = std::make_unique<ecs::ComponentRegistry>(maxEntities);
    Register<ecs::detail::FreeComponentGroup>(*registry, maxEntities, FreeComponentGroupId, "");
    Register<Tag>(*registry, maxEntities, TagId, "Tag", editor::TagUIWidget);
    Register<Transform>(*registry, maxEntities, TransformId, "Transform", editor::TransformUIWidget);
    Register<graphics::MeshRenderer>(*registry, maxEntities, MeshRendererId, "MeshRenderer", editor::MeshRendererUIWidget, CreateMeshRenderer, SerializeMeshRenderer, DeserializeMeshRenderer);
    Register<graphics::ToonRenderer>(*registry, maxEntities, ToonRendererId, "ToonRenderer", editor::ToonRendererUIWidget, CreateToonRenderer, SerializeToonRenderer, DeserializeToonRenderer);
    Register<graphics::WireframeRenderer>(*registry, maxEntities, WireframeRendererId, "WireframeRenderer");
    Register<graphics::SkeletalAnimator>(*registry, maxEntities, SkeletalAnimatorId, "SkeletalAnimator", editor::SkeletalAnimatorUIWidget, CreateSkeletalAnimator);
    Register<graphics::PointLight>(*registry, maxEntities, PointLightId, "PointLight", editor::PointLightUIWidget, CreatePointLight, SerializePointLight, DeserializePointLight);
    Register<graphics::ParticleEmitter>(*registry, maxEntities, ParticleEmitterId, "ParticleEmitter", editor::ParticleEmitterUIWidget, CreateParticleEmitter, SerializeParticleEmitter, DeserializeParticleEmitter);
    Register<physics::Collider>(*registry, maxEntities, ColliderId, "Collider", editor::ColliderUIWidget, CreateCollider, SerializeCollider, DeserializeCollider);
    Register<physics::ConcaveCollider>(*registry, maxEntities, ConcaveColliderId, "ConcaveCollider", editor::ConcaveColliderUIWidget, nullptr, SerializeConcaveCollider, DeserializeConcaveCollider);
    Register<physics::PhysicsBody>(*registry, maxEntities, PhysicsBodyId, "PhysicsBody", editor::PhysicsBodyUIWidget, CreatePhysicsBody, SerializePhysicsBody, DeserializePhysicsBody, static_cast<void*>(registry.get()));
    Register<FrameLogic>(*registry, maxEntities, FrameLogicId, "FrameLogic", editor::FrameLogicUIWidget, CreateFrameLogic);
    Register<FixedLogic>(*registry, maxEntities, FixedLogicId, "FixedLogic", editor::FixedLogicUIWidget, CreateFixedLogic);
    Register<CollisionLogic>(*registry, maxEntities, CollisionLogicId, "CollisionLogic", editor::CollisionLogicUIWidget, CreateCollisionLogic);
    Register<audio::AudioSource>(*registry, maxEntities, AudioSourceId, "AudioSource", editor::AudioSourceUIWidget, CreateAudioSource, SerializeAudioSource, DeserializeAudioSource);
    Register<net::NetworkDispatcher>(*registry, maxEntities, NetworkDispatcherId, "NetworkDispatcher", editor::NetworkDispatcherUIWidget, CreateNetworkDispatcher);
    return registry;
}
} // namespace nc
