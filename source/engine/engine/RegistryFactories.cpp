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
template<class T>
void Register(nc::Registry& registry, void(*drawUI)(T&))
{
    registry.RegisterComponentType<T>(nc::ComponentHandler<T>
    {
        .name = T::NcEditorComponentName,
        .drawUI = drawUI
    });
}

template<class T>
void Set(nc::Registry& registry, void(*drawUI)(T&))
{
    auto& handler = registry.Handler<T>();
    handler.name = T::NcEditorComponentName;
    handler.drawUI = drawUI;
}
} // anonymous namespace

namespace nc
{
auto BuildRegistry(size_t maxEntities) -> Registry
{
    NC_LOG_INFO("Building registry");
    auto registry = Registry{maxEntities};
    Set<Transform>(registry, editor::TransformUIWidget);
    Set<Tag>(registry, editor::TagUIWidget);

    Register<CollisionLogic>(registry, editor::CollisionLogicUIWidget);
    Register<FrameLogic>(registry, editor::FrameLogicUIWidget);
    Register<FixedLogic>(registry, editor::FixedLogicUIWidget);
    Register<audio::AudioSource>(registry, editor::AudioSourceUIWidget);
    Register<graphics::MeshRenderer>(registry, editor::MeshRendererUIWidget);
    Register<graphics::ParticleEmitter>(registry, editor::ParticleEmitterUIWidget);
    Register<graphics::PointLight>(registry, editor::PointLightUIWidget);
    Register<graphics::ToonRenderer>(registry, editor::ToonRendererUIWidget);
    Register<net::NetworkDispatcher>(registry, editor::NetworkDispatcherUIWidget);
    Register<physics::Collider>(registry, editor::ColliderUIWidget);
    Register<physics::ConcaveCollider>(registry, editor::ConcaveColliderUIWidget);
    Register<physics::PhysicsBody>(registry, editor::PhysicsBodyUIWidget);
    return registry;
}

auto BuildModuleRegistry(Registry* registry,
                         window::WindowImpl* window,
                         const config::Config& config) -> ModuleRegistry
{
    NC_LOG_INFO("Building module registry");
    auto moduleRegistry = nc::ModuleRegistry{};
    moduleRegistry.Register(nc::asset::BuildAssetModule(config.assetSettings, config.memorySettings));
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
