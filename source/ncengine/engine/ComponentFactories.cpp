#include "ComponentFactories.h"
#include "ncengine/asset/DefaultAssets.h"
#include "ncengine/audio/AudioSource.h"
#include "ncengine/ecs/FrameLogic.h"
#include "ncengine/ecs/ComponentRegistry.h"
#include "ncengine/graphics/DirectionalLight.h"
#include "ncengine/graphics/ParticleEmitter.h"
#include "ncengine/graphics/PointLight.h"
#include "ncengine/graphics/Mesh.h"
#include "ncengine/graphics/SpotLight.h"
#include "ncengine/physics/Constraints.h"
#include "ncengine/physics/RigidBody.h"

/** @todo 353 Remove once NcAsset has the required functionality. */
#include "asset/AssetService.h"

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

auto CreateStaticMesh(Entity entity, const std::any&) -> StaticMesh
{
    /** @todo 353 Once NcAsset has the required functionality, we should be fetching assets from it through the component context. */
    auto meshService = asset::AssetService<asset::MeshView>::Get();
    auto textureService = asset::AssetService<asset::TextureView>::Get();
    NC_ASSERT(meshService && textureService, "Asset services not registered");
    return StaticMesh{
        entity,
        meshService->Acquire(asset::CubeMesh),
        MaterialDesc{
            .properties = MaterialProperties{
                .diffuseTexture = textureService->Acquire(asset::DefaultBaseColor),
                .normalTexture = textureService->Acquire(asset::DefaultNormal)
            }
        }
    };
}

auto CreateSkinnedMesh(Entity entity, const std::any&) -> SkinnedMesh
{
    /** @todo 353 Once NcAsset has the required functionality, we should be fetching assets from it through the component context. */
    auto meshService = asset::AssetService<asset::MeshView>::Get();
    auto textureService = asset::AssetService<asset::TextureView>::Get();
    NC_ASSERT(meshService && textureService, "Asset services not registered");
    return SkinnedMesh{
        entity,
        meshService->Acquire(asset::CubeMesh),
        MaterialDesc{
            .properties = MaterialProperties{
                .diffuseTexture = textureService->Acquire(asset::DefaultBaseColor),
                .normalTexture = textureService->Acquire(asset::DefaultNormal)
            }
        }
    };
}

auto CreateSpotLight(Entity, const std::any&) -> graphics::SpotLight
{
    return graphics::SpotLight{};
}

auto CreateRigidBody(Entity entity, const std::any&) -> RigidBody
{
    return RigidBody{entity};
}

auto CreateDirectionalLight(Entity, const std::any&) -> graphics::DirectionalLight
{
    return graphics::DirectionalLight{};
}
} // namespace nc
