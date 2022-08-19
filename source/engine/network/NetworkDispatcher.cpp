#include "network/NetworkDispatcher.h"
#include "debug/NcError.h"

#ifdef NC_EDITOR_ENABLED
#include "imgui/imgui.h"
#endif

namespace nc::net
{
NetworkDispatcher::NetworkDispatcher(Entity entity) noexcept
    : ComponentBase(entity)
{
}

void NetworkDispatcher::Dispatch(PacketType packetType, uint8_t* data)
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

void NetworkDispatcher::AddHandler(PacketType packetType, std::function<void(uint8_t* data)> func)
{
    m_dispatchTable[packetType] = func;
}
} // namespace nc::net

namespace nc
{
#ifdef NC_EDITOR_ENABLED
template<>
void ComponentGuiElement<net::NetworkDispatcher>(net::NetworkDispatcher*)
{
    ImGui::Text("NetworkDispatcher");
}
#endif
} // namespace nc