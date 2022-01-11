#pragma once

#include "GenericProxy.h"
#include "ecs/Registry.h"

#include <vector>

namespace nc::physics
{
    /** Copies all object data each step. */
    class PerFrameProxyCache
    {
        public:
            using proxy_type = GenericProxy;

            PerFrameProxyCache(Registry* registry);

            void Update();
            auto Proxies() -> std::span<proxy_type> { return m_proxies; }

        private:
            Registry* m_registry;
            std::vector<proxy_type> m_proxies;
    };
}