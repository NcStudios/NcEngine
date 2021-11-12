#include "PerFrameProxyCache.h"
#include "ecs/component/PhysicsBody.h"

namespace nc::physics
{
    PerFrameProxyCache::PerFrameProxyCache(Registry* registry)
        : m_registry{registry},
          m_proxies{}
    {
    }

    void PerFrameProxyCache::Update()
    {
        const auto colliders = m_registry->ViewAll<Collider>();
        const auto colliderCount = colliders.size();
        m_proxies.clear();
        m_proxies.reserve(colliderCount);

        for(auto& collider : colliders)
        {
            auto entity = collider.ParentEntity();
            auto matrix = m_registry->Get<Transform>(entity)->TransformationMatrix();
            auto volume = collider.GetVolume();
            auto estimate = collider.EstimateBoundingVolume(matrix);
            auto body = m_registry->Get<PhysicsBody>(entity);
            auto interactionType = GetColliderInteractionType(collider.IsTrigger(), body);
            m_proxies.emplace_back(matrix, volume, estimate, entity, interactionType, nullptr);
        }
    }
}