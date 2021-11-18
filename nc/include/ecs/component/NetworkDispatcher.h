#pragma once

#include "Component.h"
#include "network/NetworkDetails.h"

#include <functional>
#include <unordered_map>

namespace nc
{
    class NetworkDispatcher final : public ComponentBase
    {
        NC_ENABLE_IN_EDITOR(NetworkDispatcher)
        
        public:
            net::NetworkHandle networkHandle;

            NetworkDispatcher(Entity entity) noexcept;
            ~NetworkDispatcher() = default;
            NetworkDispatcher(const NetworkDispatcher&) = delete;
            NetworkDispatcher(NetworkDispatcher&&) = default;
            NetworkDispatcher& operator=(const NetworkDispatcher&) = delete;
            NetworkDispatcher& operator=(NetworkDispatcher&&) = default;

            void Dispatch(net::PacketType packetType, uint8_t* data);
            void AddHandler(net::PacketType packetType, std::function<void(uint8_t* data)> func);

        private:
            std::unordered_map<net::PacketType, std::function<void(uint8_t*)>> m_dispatchTable;
    };

    #ifdef NC_EDITOR_ENABLED
    template<>
    void ComponentGuiElement<NetworkDispatcher>(NetworkDispatcher*);
    #endif
}