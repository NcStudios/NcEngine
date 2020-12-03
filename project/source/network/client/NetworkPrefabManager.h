#pragma once

#include "NcCommonTypes.h"
#include "nc/source/net/NetworkDetails.h"

#include <vector>

namespace nc { class NetworkDispatcher; }
namespace project::prefab { enum class Resource : uint32_t; }
namespace project::network
{
    struct PacketSpawnPrefab;

    class NetworkPrefabManager
    {
        public:
            void Spawn(PacketSpawnPrefab& packet);
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

            //uint32_t m_nextHandle = 0u;
    };
}