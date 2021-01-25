#include "NetworkDispatcher.h"

#include <stdexcept>

#ifdef NC_EDITOR_ENABLED
#include "imgui/imgui.h"
#endif

namespace nc
{
    NetworkDispatcher::NetworkDispatcher(EntityHandle handle) noexcept
        : ComponentBase(handle)
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

    #ifdef NC_EDITOR_ENABLED
    void NetworkDispatcher::EditorGuiElement()
    {
        ImGui::Text("NetworkDispatcher");
    }
    #endif
}