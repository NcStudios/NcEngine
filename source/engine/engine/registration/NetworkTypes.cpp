#include "NetworkTypes.h"
#include "ncengine/network/NetworkDispatcher.h"

namespace nc
{
void RegisterNetworkTypes(ecs::ComponentRegistry& registry, size_t maxEntities)
{
    Register<net::NetworkDispatcher>(registry, maxEntities, NetworkDispatcherId, "NetworkDispatcher", ui::editor::NetworkDispatcherUIWidget, CreateNetworkDispatcher);
}
} // namespace nc
