#include "AssetDependencyChecker.h"
#include "AssetManifest.h"
#include "utility/Output.h"

namespace nc::editor
{
    AssetDependencyChecker::AssetDependencyChecker(const registry_type* registry, const AssetManifest* manifest)
        : missingDependencies{},
          result{false}
    {
        for(const auto& audioSource : registry->ViewAll<AudioSource>())
        {
            if(!manifest->Contains(audioSource.m_soundClipPath, AssetType::AudioClip))
                missingDependencies.push_back(audioSource.m_soundClipPath);
        }

        for(const auto& collider : registry->ViewAll<Collider>())
        {
            if(collider.GetType() != ColliderType::Hull)
                continue;
            
            if(!manifest->ContainsNca(collider.m_info.hullAssetPath, AssetType::HullCollider))
                missingDependencies.push_back(collider.m_info.hullAssetPath);
        }

        for(const auto& collider : registry->ViewAll<ConcaveCollider>())
        {
            if(!manifest->ContainsNca(collider.m_path, AssetType::ConcaveCollider))
                missingDependencies.push_back(collider.m_path);
        }

        for(const auto& renderer : registry->ViewAll<MeshRenderer>())
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
}