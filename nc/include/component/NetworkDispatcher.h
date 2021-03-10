#pragma once

#include "Component.h"
#include "network/NetworkDetails.h"

#include <functional>
#include <unordered_map>

namespace nc
{
    class NetworkDispatcher final : public ComponentBase
    {
        public:
            net::NetworkHandle networkHandle;

            NetworkDispatcher(EntityHandle handle) noexcept;
            ~NetworkDispatcher() = default;
            NetworkDispatcher(const NetworkDispatcher&) = delete;
            NetworkDispatcher(NetworkDispatcher&&) = delete;
            NetworkDispatcher& operator=(const NetworkDispatcher&) = delete;
            NetworkDispatcher& operator=(NetworkDispatcher&&) = delete;

            void Dispatch(net::PacketType packetType, uint8_t* data);
            void AddHandler(net::PacketType packetType, std::function<void(uint8_t* data)> func);

            #ifdef NC_EDITOR_ENABLED
            void EditorGuiElement() override;
            #endif

        private:
            std::unordered_map<net::PacketType, std::function<void(uint8_t*)>> m_dispatchTable;
    };
}