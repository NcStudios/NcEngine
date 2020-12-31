#include "NetworkDispatcher.h"

#include <stdexcept>

namespace nc
{
    NetworkDispatcher::NetworkDispatcher(ComponentHandle handle, EntityHandle parentHandle) noexcept
        : Component(handle, parentHandle)
    {
    }

    void NetworkDispatcher::Dispatch(net::PacketType packetType, uint8_t* data)
    {
        try
        {
            m_dispatchTable.at(packetType)(data);
        }
        catch(std::out_of_range& e)
        {
            throw std::runtime_error("NetworkDispatcher::Dispatch - Unknown packet type");
        }
    }

    void NetworkDispatcher::AddHandler(net::PacketType packetType, std::function<void(uint8_t* data)> func)
    {
        m_dispatchTable[packetType] = func;
    }
}