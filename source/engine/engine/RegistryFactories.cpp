#include "RegistryFactories.h"
#include "ComponentFactories.h"
#include "ncengine/asset/DefaultAssets.h"
#include "ncengine/asset/NcAsset.h"
#include "ncengine/audio/AudioSource.h"
#include "ncengine/config/Config.h"
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
#include "ncengine/module/ModuleRegistry.h"
#include "ncengine/network/NetworkDispatcher.h"
#include "ncengine/physics/Collider.h"
#include "ncengine/physics/ConcaveCollider.h"
#include "ncengine/physics/PhysicsBody.h"
#include "ncengine/utility/Log.h"
#include "ui/editor/ComponentWidgets.h"

/** @todo #363 Move factories to public headers and include those instead. */
#include "audio/NcAudioImpl.h"
#include "ecs/EcsModule.h"
#include "graphics/NcGraphicsImpl.h"
#include "physics/NcPhysicsImpl.h"
#include "time/TimeImpl.h"

namespace
{
auto BuildDefaultAssetMap() -> nc::asset::AssetMap
{
    using namespace nc::asset;
    return AssetMap
    {
        { AssetType::AudioClip,       { DefaultAudioClip } },
        { AssetType::ConcaveCollider, { DefaultConcaveCollider } },
        { AssetType::CubeMap,         { DefaultSkyboxCubeMap } },
        { AssetType::HullCollider,    { DefaultHullCollider } },
        { AssetType::Mesh,            { PlaneMesh, CubeMesh, SphereMesh, CapsuleMesh } },
        { AssetType::Texture,         { DefaultBaseColor, DefaultNormal, DefaultRoughness, DefaultParticle } }
    };
}

template<class T>
void Register(nc::Registry& registry,
              const char* name,
              typename nc::ComponentHandler<T>::DrawUI_t&& drawUI = nullptr,
              typename nc::ComponentHandler<T>::Factory_t&& factory = nullptr,
              void* userData = nullptr)
{
    registry.RegisterComponentType<T>(nc::ComponentHandler<T>
    {
        .name = name,
        .userData = userData,
        .factory = std::move(factory),
        .drawUI = std::move(drawUI)
    });
}

template<class T>
void Set(nc::Registry& registry,
         const char* name,
         typename nc::ComponentHandler<T>::DrawUI_t&& drawUI = nullptr)
{
    auto& handler = registry.Handler<T>();
    handler.name = name;
    handler.drawUI = std::move(drawUI);
}
} // anonymous namespace

namespace nc
{
auto BuildRegistry(size_t maxEntities) -> std::unique_ptr<Registry>
{
    NC_LOG_INFO("Building registry");
    auto registry = std::make_unique<Registry>(maxEntities);
    Set<Transform>(*registry, "Transform", editor::TransformUIWidget);
    Set<Tag>(*registry, "Tag", editor::TagUIWidget);

    Register<CollisionLogic>(*registry, "CollisionLogic", editor::CollisionLogicUIWidget, CreateCollisionLogic);
    Register<FrameLogic>(*registry, "FrameLogic", editor::FrameLogicUIWidget, CreateFrameLogic);
    Register<FixedLogic>(*registry, "FixedLogic", editor::FixedLogicUIWidget, CreateFixedLogic);
    Register<audio::AudioSource>(*registry, "AudioSource", editor::AudioSourceUIWidget, CreateAudioSource);
    Register<graphics::MeshRenderer>(*registry, "MeshRenderer", editor::MeshRendererUIWidget, CreateMeshRenderer);
    Register<graphics::ParticleEmitter>(*registry, "ParticleEmitter", editor::ParticleEmitterUIWidget, CreateParticleEmitter);
    Register<graphics::PointLight>(*registry, "PointLight", editor::PointLightUIWidget);
    Register<graphics::ToonRenderer>(*registry, "ToonRenderer", editor::ToonRendererUIWidget, CreateToonRenderer);
    Register<net::NetworkDispatcher>(*registry, "NetworkDispatcher", editor::NetworkDispatcherUIWidget, CreateNetworkDispatcher);
    Register<physics::Collider>(*registry, "Collider", editor::ColliderUIWidget, CreateCollider);
    Register<physics::ConcaveCollider>(*registry, "ConcaveCollider", editor::ConcaveColliderUIWidget);
    Register<physics::PhysicsBody>(*registry, "PhysicsBody", editor::PhysicsBodyUIWidget, CreatePhysicsBody, static_cast<void*>(registry.get()));
    return registry;
}

auto BuildModuleRegistry(Registry* registry,
                         window::WindowImpl* window,
                         const config::Config& config) -> ModuleRegistry
{
    NC_LOG_INFO("Building module registry");
    auto moduleRegistry = nc::ModuleRegistry{};
    moduleRegistry.Register(nc::asset::BuildAssetModule(config.assetSettings,
                                                        config.memorySettings,
                                                        BuildDefaultAssetMap()));
    moduleRegistry.Register(nc::graphics::BuildGraphicsModule(config.projectSettings,
                                                              config.graphicsSettings,
                                                              moduleRegistry.Get<nc::asset::NcAsset>(),
                                                              registry,
                                                              window));
    moduleRegistry.Register(nc::physics::BuildPhysicsModule(config.physicsSettings, registry));
    moduleRegistry.Register(nc::audio::BuildAudioModule(config.audioSettings, registry));
    moduleRegistry.Register(nc::time::BuildTimeModule());
    moduleRegistry.Register(nc::ecs::BuildEcsModule(registry));
    moduleRegistry.Register(std::make_unique<nc::Random>());
    return moduleRegistry;
}
} // namespace nc
