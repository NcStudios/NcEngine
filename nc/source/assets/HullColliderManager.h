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

    class HullColliderManager
    {
        public:
            HullColliderManager() { HullColliderManager::instance = this; }

            static bool Load(const std::string& uid, HullColliderFlyweight data)
            {
                return Get()->Load_(uid, std::move(data));
            }
            
            static bool IsLoaded(const std::string& uid)
            {
                return Get()->IsLoaded_(uid);
            }

            static auto Acquire(const std::string& uid) -> HullCollider
            {
                return Get()->Acquire_(uid);
            }

        private:
            static inline HullColliderManager* instance = nullptr;
            std::unordered_map<std::string, HullColliderFlyweight> m_resources;

            static auto Get() -> HullColliderManager*
            {
                return HullColliderManager::instance;
            }

            bool Load_(const std::string& uid, HullColliderFlyweight data)
            {
                if(IsLoaded(uid))
                    return false;
                
                m_resources.emplace(uid, std::move(data));
                return true;
            }

            bool IsLoaded_(const std::string& uid) const
            {
                return m_resources.end() != m_resources.find(uid);
            }
            
            auto Acquire_(const std::string& uid) const -> HullCollider
            {
                const auto it = m_resources.find(uid);
                if(it == m_resources.end())
                    throw std::runtime_error("HullColliderManager::Acquire_ - Resource is not loaded: " + uid);

                return HullCollider{std::span<const Vector3>{it->second.vertices}, it->second.extents, it->second.maxExtent};
            }
    };
} // namespace nc