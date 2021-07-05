#pragma once

#include "physics/collision/CollisionVolumes.h"

#include <span>
#include <stdexcept>
#include <string>
#include <vector>
#include <unordered_map>

namespace nc
{
    struct HullColliderFlyweight
    {
        std::vector<Vector3> vertices;
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

            static auto Acquire(const std::string& uid) -> physics::HullCollider
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
            
            auto Acquire_(const std::string& uid) const -> physics::HullCollider
            {
                const auto it = m_resources.find(uid);
                if(it == m_resources.end())
                    throw std::runtime_error("HullColliderManager::Acquire_ - Resource is not loaded: " + uid);

                return physics::HullCollider{std::span<const Vector3>{it->second.vertices}, it->second.maxExtent};
            }
    };
} // namespace nc