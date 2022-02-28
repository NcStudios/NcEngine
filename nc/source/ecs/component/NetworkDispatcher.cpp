#include "ecs/component/NetworkDispatcher.h"
#include "debug/NcError.h"

#ifdef NC_EDITOR_ENABLED
#include "imgui/imgui.h"
#endif

namespace nc
{
    NetworkDispatcher::NetworkDispatcher(Entity entity) noexcept
        : ComponentBase(entity)
    {
    }

    void NetworkDispatcher::Dispatch(net::PacketType packetType, uint8_t* data)
    {
        try
        {
            m_dispatchTable.at(packetType)(data);
        }
        catch(std::out_of_range&)
        {
            throw NcError("Unknown PacketType");
        }
    }

    void NetworkDispatcher::AddHandler(net::PacketType packetType, std::function<void(uint8_t* data)> func)
    {
        m_dispatchTable[packetType] = func;
    }

    #ifdef NC_EDITOR_ENABLED
    template<>
    void ComponentGuiElement<NetworkDispatcher>(NetworkDispatcher*)
    {
        ImGui::Text("NetworkDispatcher");
    }
    #endif
}