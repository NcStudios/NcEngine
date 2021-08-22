#pragma once

#include "component/Collider.h"

#include <stdexcept>
#include <vector>
#include <unordered_map>

namespace nc
{
    struct HullColliderFlyweight
    {
        std::vector<Vector3> vertices;
        Vector3 extents;
        float maxExtent;
    };

    struct MeshColliderFlyweight
    {
        std::vector<MeshCollider::Triangle> triangles;
    };

    /** @todo Mesh assets are managed elsewhere. Can we merge everything together? */

    class AssetManager
    {
        public:
            AssetManager() { AssetManager::instance = this; }

            static bool LoadHullCollider(const std::string& uid, HullColliderFlyweight data)
            {
                return Get()->LoadHullCollider_(uid, std::move(data));
            }
            
            static bool LoadMeshCollider(const std::string& uid, MeshColliderFlyweight data)
            {
                return Get()->LoadMeshCollider_(uid, std::move(data));
            }

            static bool IsHullColliderLoaded(const std::string& uid)
            {
                return Get()->IsHullColliderLoaded_(uid);
            }

            static bool IsMeshColliderLoaded(const std::string& uid)
            {
                return Get()->IsMeshColliderLoaded_(uid);
            }

            static auto AcquireHullCollider(const std::string& uid) -> HullCollider
            {
                return Get()->AcquireHullCollider_(uid);
            }

            static auto AcquireMeshCollider(const std::string& uid) -> MeshCollider
            {
                return Get()->AcquireMeshCollider_(uid);
            }

        private:
            static inline AssetManager* instance = nullptr;
            std::unordered_map<std::string, HullColliderFlyweight> m_hullColliders;
            std::unordered_map<std::string, MeshColliderFlyweight> m_meshColliders;

            static auto Get() -> AssetManager*
            {
                return AssetManager::instance;
            }

            bool LoadHullCollider_(const std::string& uid, HullColliderFlyweight data)
            {
                if(IsHullColliderLoaded_(uid))
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

            bool IsHullColliderLoaded_(const std::string& uid) const
            {
                return m_hullColliders.end() != m_hullColliders.find(uid);
            }
            
            bool IsMeshColliderLoaded_(const std::string& uid) const
            {
                return m_meshColliders.end() != m_meshColliders.find(uid);
            }

            auto AcquireHullCollider_(const std::string& uid) const -> HullCollider
            {
                const auto it = m_hullColliders.find(uid);
                if(it == m_hullColliders.end())
                    throw std::runtime_error("AssetManager::AcquireHullCollider_ - Resource is not loaded: " + uid);

                return HullCollider{std::span<const Vector3>{it->second.vertices}, it->second.extents, it->second.maxExtent};
            }

            auto AcquireMeshCollider_(const std::string& uid) const -> MeshCollider
            {
                const auto it = m_meshColliders.find(uid);
                if(it == m_meshColliders.end())
                    throw std::runtime_error("AssetManager::AcquireMeshCollider_ - Resource is not loaded: " + uid);
                
                return MeshCollider{};
            }
    };
} // namespace nc