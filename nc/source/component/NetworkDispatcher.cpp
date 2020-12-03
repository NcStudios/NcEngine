#include "NetworkDispatcher.h"

#include <stdexcept>

namespace nc
{
    void NetworkDispatcher::Dispatch(net::PacketType packetType, uint8_t* data)
    {
        auto pos = m_dispatchTable.find(packetType);
        if(pos == m_dispatchTable.end())
        {
            throw std::runtime_error("NetworkDispatcher::Dispatch - Unknonwn packet type");
        }
        pos->second(data);
        
        //can probably just do:
        //m_dispatchTable.at[packetType]();
    }

    void NetworkDispatcher::AddHandler(net::PacketType packetType, std::function<void(uint8_t* data)> func)
    {
        m_dispatchTable[packetType] = func;
    }
}