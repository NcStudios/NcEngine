#include "ModuleFactory.h"

#include "ncengine/Events.h"
#include "ncengine/asset/DefaultAssets.h"
#include "ncengine/asset/NcAsset.h"
#include "ncengine/audio/NcAudio.h"
#include "ncengine/config/Config.h"
#include "ncengine/ecs/NcEcs.h"
#include "ncengine/ecs/Registry.h"
#include "ncengine/graphics/NcGraphics.h"
#include "ncengine/math/Random.h"
#include "ncengine/module/ModuleRegistry.h"
#include "ncengine/physics/NcPhysics.h"
#include "ncengine/scene/NcScene.h"
#include "ncengine/utility/Log.h"

namespace
{
auto BuildDefaultAssetMap() -> nc::asset::AssetMap
{
    using namespace nc::asset;
    return AssetMap
    {
        { AssetType::AudioClip,         { DefaultAudioClip } },
        { AssetType::ConcaveCollider,   { DefaultConcaveCollider } },
        { AssetType::CubeMap,           { DefaultSkyboxCubeMap } },
        { AssetType::HullCollider,      { DefaultHullCollider } },
        { AssetType::Mesh,              { PlaneMesh, CubeMesh, SphereMesh, CapsuleMesh } },
        { AssetType::SkeletalAnimation, { DefaultSkeletalAnimation } },
        { AssetType::Texture,           { DefaultBaseColor, DefaultNormal, DefaultRoughness, DefaultParticle } }
    };
}
} // anonymous namespace

namespace nc
{
auto BuildModuleRegistry(Registry* registry,
                         SystemEvents& events,
                         const config::Config& config) -> std::unique_ptr<ModuleRegistry>
{
    NC_LOG_INFO("Building module registry");
    auto moduleRegistry = std::make_unique<nc::ModuleRegistry>();
    moduleRegistry->Register(nc::window::BuildWindowModule(config.projectSettings,
                                                           config.graphicsSettings,
                                                           events.quit));
    moduleRegistry->Register(nc::BuildSceneModule());
    moduleRegistry->Register(nc::asset::BuildAssetModule(config.assetSettings,
                                                        config.memorySettings,
                                                        BuildDefaultAssetMap()));
    moduleRegistry->Register(nc::graphics::BuildGraphicsModule(config.projectSettings,
                                                              config.graphicsSettings,
                                                              config.memorySettings,
                                                              ModuleProvider{moduleRegistry.get()},
                                                              registry,
                                                              events));
    moduleRegistry->Register(nc::physics::BuildPhysicsModule(config.physicsSettings, registry, events));
    moduleRegistry->Register(nc::audio::BuildAudioModule(config.audioSettings, registry->GetEcs()));
    moduleRegistry->Register(nc::ecs::BuildEcsModule(registry->GetImpl(), events));
    moduleRegistry->Register(std::make_unique<nc::Random>());
    return moduleRegistry;
}
} // namespace nc
