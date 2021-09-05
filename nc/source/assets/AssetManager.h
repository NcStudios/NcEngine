#pragma once

#include "component/Collider.h"

#include <stdexcept>
#include <vector>
#include <unordered_map>

namespace nc
{
    struct SoundClipFlyweight
    {
        std::vector<double> leftChannel;
        std::vector<double> rightChannel;
        size_t samplesPerChannel;
    };

    struct ConvexHullFlyweight
    {
        std::vector<Vector3> vertices;
        Vector3 extents;
        float maxExtent;
    };

    struct MeshColliderFlyweight
    {
        std::vector<Triangle> triangles;
        float maxExtent;
    };

    /** @todo Mesh assets are managed elsewhere. Can we merge everything together? */

    class AssetManager
    {
        public:
            AssetManager() { AssetManager::instance = this; }

            static bool LoadSoundClip(const std::string& uid, SoundClipFlyweight data)
            {
                return Get()->LoadSoundClip_(uid, std::move(data));
            }

            static bool LoadConvexHull(const std::string& uid, ConvexHullFlyweight data)
            {
                return Get()->LoadConvexHull_(uid, std::move(data));
            }
            
            static bool LoadMeshCollider(const std::string& uid, MeshColliderFlyweight data)
            {
                return Get()->LoadMeshCollider_(uid, std::move(data));
            }

            static bool IsSoundClipLoaded(const std::string& uid)
            {
                return Get()->IsSoundClipLoaded_(uid);
            }

            static bool IsConvexHullLoaded(const std::string& uid)
            {
                return Get()->IsConvexHullLoaded_(uid);
            }

            static bool IsMeshColliderLoaded(const std::string& uid)
            {
                return Get()->IsMeshColliderLoaded_(uid);
            }

            static auto AcquireSoundClip(const std::string& uid) -> const SoundClipFlyweight&
            {
                return Get()->AcquireSoundClip_(uid);
            }

            static auto AcquireConvexHull(const std::string& uid) -> ConvexHull
            {
                return Get()->AcquireConvexHull_(uid);
            }

            static auto AcquireMeshCollider(const std::string& uid) -> const MeshColliderFlyweight&
            {
                return Get()->AcquireMeshCollider_(uid);
            }

        private:
            static inline AssetManager* instance = nullptr;
            std::unordered_map<std::string, SoundClipFlyweight> m_soundClips;
            std::unordered_map<std::string, ConvexHullFlyweight> m_hullColliders;
            std::unordered_map<std::string, MeshColliderFlyweight> m_meshColliders;

            static auto Get() -> AssetManager*
            {
                return AssetManager::instance;
            }

            bool LoadSoundClip_(const std::string& uid, SoundClipFlyweight data)
            {
                if(IsSoundClipLoaded_(uid))
                    return false;
                
                m_soundClips.emplace(uid, std::move(data));
                return true;
            }

            bool LoadConvexHull_(const std::string& uid, ConvexHullFlyweight data)
            {
                if(IsConvexHullLoaded_(uid))
                    return false;
                
                m_hullColliders.emplace(uid, std::move(data));
                return true;
            }

            bool LoadMeshCollider_(const std::string& uid, MeshColliderFlyweight data)
            {
                if(IsMeshColliderLoaded_(uid))
                    return false;
                
                m_meshColliders.emplace(uid, std::move(data));
                return true;
            }

            bool IsSoundClipLoaded_(const std::string& uid) const
            {
                return m_soundClips.end() != m_soundClips.find(uid);
            }

            bool IsConvexHullLoaded_(const std::string& uid) const
            {
                return m_hullColliders.end() != m_hullColliders.find(uid);
            }
            
            bool IsMeshColliderLoaded_(const std::string& uid) const
            {
                return m_meshColliders.end() != m_meshColliders.find(uid);
            }

            auto AcquireSoundClip_(const std::string& uid) const -> const SoundClipFlyweight&
            {
                const auto it = m_soundClips.find(uid);
                if(it == m_soundClips.end())
                    throw std::runtime_error("AssetManager::AcquireSoundClip_ - Resource is not loaded: " + uid);

                return it->second;
            }

            auto AcquireConvexHull_(const std::string& uid) const -> ConvexHull
            {
                const auto it = m_hullColliders.find(uid);
                if(it == m_hullColliders.end())
                    throw std::runtime_error("AssetManager::AcquireConvexHull_ - Resource is not loaded: " + uid);

                return ConvexHull{std::span<const Vector3>{it->second.vertices}, it->second.extents, it->second.maxExtent};
            }

            auto AcquireMeshCollider_(const std::string& uid) const -> const MeshColliderFlyweight&
            {
                const auto it = m_meshColliders.find(uid);
                if(it == m_meshColliders.end())
                    throw std::runtime_error("AssetManager::AcquireMeshCollider_ - Resource is not loaded: " + uid);
                
                return it->second;
            }
    };
} // namespace nc