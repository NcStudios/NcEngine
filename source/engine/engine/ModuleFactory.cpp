#include "ModuleFactory.h"

#include "ncengine/asset/DefaultAssets.h"
#include "ncengine/asset/NcAsset.h"
#include "ncengine/config/Config.h"
#include "ncengine/ecs/Registry.h"
#include "ncengine/module/ModuleRegistry.h"
#include "ncengine/scene/NcScene.h"
#include "ncengine/utility/Log.h"

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
} // anonymous namespace

namespace nc
{
auto BuildModuleRegistry(Registry* registry,
                         window::WindowImpl* window,
                         const config::Config& config) -> std::unique_ptr<ModuleRegistry>
{
    NC_LOG_INFO("Building module registry");
    auto moduleRegistry = std::make_unique<nc::ModuleRegistry>();
    moduleRegistry->Register(nc::BuildSceneModule());
    moduleRegistry->Register(nc::asset::BuildAssetModule(config.assetSettings,
                                                        config.memorySettings,
                                                        BuildDefaultAssetMap()));
    moduleRegistry->Register(nc::graphics::BuildGraphicsModule(config.projectSettings,
                                                              config.graphicsSettings,
                                                              ModuleProvider{moduleRegistry.get()},
                                                              registry,
                                                              window));
    moduleRegistry->Register(nc::physics::BuildPhysicsModule(config.physicsSettings, registry));
    moduleRegistry->Register(nc::audio::BuildAudioModule(config.audioSettings, registry));
    moduleRegistry->Register(nc::time::BuildTimeModule());
    moduleRegistry->Register(nc::ecs::BuildEcsModule(registry));
    moduleRegistry->Register(std::make_unique<nc::Random>());
    return moduleRegistry;
}
} // namespace nc
