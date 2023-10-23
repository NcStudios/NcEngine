#include "RegistryFactories.h"
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
        { AssetType::AudioClip,       { nc::DefaultAudioClip } },
        { AssetType::ConcaveCollider, { nc::DefaultConcaveCollider } },
        { AssetType::CubeMap,         { nc::DefaultCubeMap } },
        { AssetType::HullCollider,    { nc::DefaultHullCollider } },
        { AssetType::Mesh,            { nc::PlaneMesh, nc::CubeMesh, nc::SphereMesh, nc::CapsuleMesh, nc::SkyboxMesh } },
        { AssetType::Texture,         { nc::DefaultBaseColor, nc::DefaultNormal, nc::DefaultParticle } }
    };
}

template<class T>
void Register(nc::Registry& registry, const char* name, void(*drawUI)(T&))
{
    registry.RegisterComponentType<T>(nc::ComponentHandler<T>
    {
        .name = name,
        .drawUI = drawUI
    });
}

template<class T>
void Set(nc::Registry& registry, const char* name, void(*drawUI)(T&))
{
    auto& handler = registry.Handler<T>();
    handler.name = name;
    handler.drawUI = drawUI;
}
} // anonymous namespace

namespace nc
{
auto BuildRegistry(size_t maxEntities) -> Registry
{
    NC_LOG_INFO("Building registry");
    auto registry = Registry{maxEntities};
    Set<Transform>(registry, "Transform", editor::TransformUIWidget);
    Set<Tag>(registry, "Tag", editor::TagUIWidget);

    Register<CollisionLogic>(registry, "CollisionLogic", editor::CollisionLogicUIWidget);
    Register<FrameLogic>(registry, "FrameLogic", editor::FrameLogicUIWidget);
    Register<FixedLogic>(registry, "FixedLogic", editor::FixedLogicUIWidget);
    Register<audio::AudioSource>(registry, "AudioSource", editor::AudioSourceUIWidget);
    Register<graphics::MeshRenderer>(registry, "MeshRenderer", editor::MeshRendererUIWidget);
    Register<graphics::ParticleEmitter>(registry, "ParticleEmitter", editor::ParticleEmitterUIWidget);
    Register<graphics::PointLight>(registry, "PointLight", editor::PointLightUIWidget);
    Register<graphics::ToonRenderer>(registry, "ToonRenderer", editor::ToonRendererUIWidget);
    Register<net::NetworkDispatcher>(registry, "NetworkDispatcher", editor::NetworkDispatcherUIWidget);
    Register<physics::Collider>(registry, "Collider", editor::ColliderUIWidget);
    Register<physics::ConcaveCollider>(registry, "ConcaveCollider", editor::ConcaveColliderUIWidget);
    Register<physics::PhysicsBody>(registry, "PhysicsBody", editor::PhysicsBodyUIWidget);
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
