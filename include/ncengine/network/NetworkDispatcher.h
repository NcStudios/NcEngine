#pragma once

#include "NetworkDetails.h"
#include "ncengine/ecs/Component.h"

#include <functional>
#include <unordered_map>

namespace nc::net
{
class NetworkDispatcher final : public ComponentBase
{
    NC_ENABLE_IN_EDITOR(NetworkDispatcher)

    public:
        static constexpr auto ComponentId = 15ull;

        NetworkHandle networkHandle;

        NetworkDispatcher(Entity entity) noexcept;
        ~NetworkDispatcher() = default;
        NetworkDispatcher(const NetworkDispatcher&) = delete;
        NetworkDispatcher(NetworkDispatcher&&) = default;
        NetworkDispatcher& operator=(const NetworkDispatcher&) = delete;
        NetworkDispatcher& operator=(NetworkDispatcher&&) = default;

        void Dispatch(PacketType packetType, uint8_t* data);
        void AddHandler(PacketType packetType, std::function<void(uint8_t* data)> func);

    private:
        std::unordered_map<PacketType, std::function<void(uint8_t*)>> m_dispatchTable;
};
} // namespace nc::net
