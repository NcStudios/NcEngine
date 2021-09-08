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

    struct ConcaveColliderFlyweight
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
            
            static bool LoadConcaveCollider(const std::string& uid, ConcaveColliderFlyweight data)
            {
                return Get()->LoadConcaveCollider_(uid, std::move(data));
            }

            static bool IsSoundClipLoaded(const std::string& uid)
            {
                return Get()->IsSoundClipLoaded_(uid);
            }

            static bool IsConvexHullLoaded(const std::string& uid)
            {
                return Get()->IsConvexHullLoaded_(uid);
            }

            static bool IsConcaveColliderLoaded(const std::string& uid)
            {
                return Get()->IsConcaveColliderLoaded_(uid);
            }

            static auto AcquireSoundClip(const std::string& uid) -> const SoundClipFlyweight&
            {
                return Get()->AcquireSoundClip_(uid);
            }

            static auto AcquireConvexHull(const std::string& uid) -> ConvexHull
            {
                return Get()->AcquireConvexHull_(uid);
            }

            static auto AcquireConcaveCollider(const std::string& uid) -> const ConcaveColliderFlyweight&
            {
                return Get()->AcquireConcaveCollider_(uid);
            }

        private:
            static inline AssetManager* instance = nullptr;
            std::unordered_map<std::string, SoundClipFlyweight> m_soundClips;
            std::unordered_map<std::string, ConvexHullFlyweight> m_hullColliders;
            std::unordered_map<std::string, ConcaveColliderFlyweight> m_concaveColliders;

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

            bool LoadConcaveCollider_(const std::string& uid, ConcaveColliderFlyweight data)
            {
                if(IsConcaveColliderLoaded_(uid))
                    return false;
                
                m_concaveColliders.emplace(uid, std::move(data));
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
            
            bool IsConcaveColliderLoaded_(const std::string& uid) const
            {
                return m_concaveColliders.end() != m_concaveColliders.find(uid);
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

            auto AcquireConcaveCollider_(const std::string& uid) const -> const ConcaveColliderFlyweight&
            {
                const auto it = m_concaveColliders.find(uid);
                if(it == m_concaveColliders.end())
                    throw std::runtime_error("AssetManager::AcquireConcaveCollider_ - Resource is not loaded: " + uid);
                
                return it->second;
            }
    };
} // namespace nc