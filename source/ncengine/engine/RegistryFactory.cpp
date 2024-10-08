#include "RegistryFactory.h"
#include "registration/AudioTypes.h"
#include "registration/CoreTypes.h"
#include "registration/GraphicsTypes.h"
#include "registration/LogicTypes.h"
#include "registration/NetworkTypes.h"
#include "registration/PhysicsTypes.h"
#include "ncengine/ecs/ComponentRegistry.h"
#include "ncengine/utility/Log.h"

namespace nc
{
auto BuildRegistry(size_t maxEntities) -> std::unique_ptr<ecs::ComponentRegistry>
{
    NC_LOG_INFO("Building registry");
    auto registry = std::make_unique<ecs::ComponentRegistry>(maxEntities);
    // We break up registration into separate object files to support mingw linker limitations
    RegisterCoreTypes(*registry, maxEntities);
    RegisterGraphicsTypes(*registry, maxEntities);
    RegisterPhysicsTypes(*registry, maxEntities);
    RegisterLogicTypes(*registry, maxEntities);
    RegisterAudioTypes(*registry, maxEntities);
    RegisterNetworkTypes(*registry, maxEntities);
    return registry;
}
} // namespace nc
