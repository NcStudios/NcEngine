#pragma once

#include "EntityHandle.h"
#include "net/NetworkDetails.h"

#include <vector>

namespace nc { class NetworkDispatcher; }
namespace project::prefab { enum class Resource : uint32_t; }
namespace project::network
{
    namespace Packet { struct SpawnPrefab; }

    class NetworkPrefabManager
    {
        public:
            void Spawn(Packet::SpawnPrefab& packet);
            void Destroy(nc::net::NetworkHandle networkHandle);
            [[nodiscard]] bool Exists(nc::net::NetworkHandle networkHandle) const;
            [[nodiscard]] nc::NetworkDispatcher* GetDispatcher(nc::net::NetworkHandle networkHandle);

        private:
            struct
            {
                std::vector<prefab::Resource> resources;
                std::vector<nc::EntityHandle> entityHandles;
                std::vector<nc::net::NetworkHandle> networkHandles;
                std::vector<nc::NetworkDispatcher*> dispatchers;
            } m_data;
    };
}