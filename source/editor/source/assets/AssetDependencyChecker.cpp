#include "AssetDependencyChecker.h"
#include "AssetManifest.h"
#include "utility/Output.h"

#include "ncengine/audio/AudioSource.h"
#include "ncengine/ecs/View.h"
#include "ncengine/graphics/MeshRenderer.h"
#include "ncengine/physics/Collider.h"
#include "ncengine/physics/ConcaveCollider.h"

namespace nc::editor
{
AssetDependencyChecker::AssetDependencyChecker(const Registry* registry, const AssetManifest* manifest)
    : missingDependencies{},
        result{false}
{
    for(const auto& audioSource : View<const audio::AudioSource>{registry})
    {
        if(!manifest->Contains(audioSource.m_audioClipPath, AssetType::AudioClip))
            missingDependencies.push_back(audioSource.m_audioClipPath);
    }

    for(const auto& collider : View<const physics::Collider>{registry})
    {
        if(collider.GetType() != physics::ColliderType::Hull)
            continue;

        if(!manifest->ContainsNca(collider.m_info.hullAssetPath, AssetType::HullCollider))
            missingDependencies.push_back(collider.m_info.hullAssetPath);
    }

    for(const auto& collider : View<const physics::ConcaveCollider>{registry})
    {
        if(!manifest->ContainsNca(collider.m_path, AssetType::ConcaveCollider))
            missingDependencies.push_back(collider.m_path);
    }

    for(const auto& renderer : View<const graphics::MeshRenderer>{registry})
    {
        if(!manifest->ContainsNca(renderer.m_meshPath, AssetType::Mesh))
            missingDependencies.push_back(renderer.m_meshPath);

        if(!manifest->Contains(renderer.m_material.baseColor, AssetType::Texture))
            missingDependencies.push_back(renderer.m_material.baseColor);

        if(!manifest->Contains(renderer.m_material.normal, AssetType::Texture))
            missingDependencies.push_back(renderer.m_material.normal);

        if(!manifest->Contains(renderer.m_material.roughness, AssetType::Texture))
            missingDependencies.push_back(renderer.m_material.roughness);
    }

    if(missingDependencies.size() == 0)
        result = true;
}

void AssetDependencyChecker::LogMissingDependencies()
{
    for(const auto& path : missingDependencies)
    {
        Output::LogError("Missing asset: " + path);
    }
}
} // namespace nc::editor
