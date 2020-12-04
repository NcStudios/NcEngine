#pragma once

#include "Component.h"
#include "net/NetworkDetails.h"

#include <functional>
#include <unordered_map>

namespace nc
{
    class NetworkDispatcher final : public Component
    {
        public:
            net::NetworkHandle networkHandle;
            
            void Dispatch(net::PacketType packetType, uint8_t* data);
            void AddHandler(net::PacketType packetType, std::function<void(uint8_t* data)> func);

        private:
            std::unordered_map<net::PacketType, std::function<void(uint8_t*)>> m_dispatchTable;
    };
}