#pragma once

#include "GenericProxy.h"
#include "physics/PhysicsBody.h"
#include "ecs/View.h"

#include <vector>

namespace nc::physics
{
/** Copies all object data each step. */
template<class ClientData>
class PerFrameProxyCache
{
    public:
        using proxy_type = GenericProxy;

        PerFrameProxyCache(ClientData* client);

        void Update();
        auto Proxies() -> std::span<proxy_type> { return m_proxies; }

    private:
        ClientData* m_client;
        std::vector<proxy_type> m_proxies;
};

template<class ClientData>
PerFrameProxyCache<ClientData>::PerFrameProxyCache(ClientData* client)
    : m_client{client},
        m_proxies{}
{
}

template<class ClientData>
void PerFrameProxyCache<ClientData>::Update()
{
    const auto colliders = View<Collider>{m_client};
    const auto colliderCount = colliders.size();
    m_proxies.clear();
    m_proxies.reserve(colliderCount);

    for(auto& collider : colliders)
    {
        auto entity = collider.ParentEntity();
        const auto& matrix = m_client->template Get<Transform>(entity)->TransformationMatrix();
        const auto& volume = collider.GetVolume();
        auto estimate = collider.EstimateBoundingVolume(matrix);
        auto properties = [client = m_client, &collider, entity]()
        {
            if (client-> template Contains<PhysicsBody>(entity))
            {
                auto body = client->template Get<PhysicsBody>(entity);
                return ClientObjectProperties{entity.IsStatic(), collider.IsTrigger(), body->IsKinematic()};
            }

            return ClientObjectProperties{entity.IsStatic(), collider.IsTrigger()};
        }();

        m_proxies.emplace_back(matrix, volume, estimate, entity, properties);
    }
}
} // namespace nc::physics
