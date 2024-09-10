#include "JoltApi.h"

#include "Jolt/Core/Factory.h"
#include "Jolt/RegisterTypes.h"

#include <memory>

namespace
{
auto g_factory = std::unique_ptr<JPH::Factory>{};
} // anonymous namespace

namespace nc::physics
{
auto JoltApi::Initialize(const task::AsyncDispatcher& dispatcher) -> JoltApi
{
    RegisterAllocator();
    g_factory = std::make_unique<JPH::Factory>();
    JPH::Factory::sInstance = g_factory.get();
    JPH::RegisterTypes();
    return JoltApi{dispatcher};
}

JoltApi::~JoltApi() noexcept
{
    JPH::UnregisterTypes();
    g_factory = nullptr;
    JPH::Factory::sInstance = nullptr;
}

JoltApi::JoltApi(const task::AsyncDispatcher& dispatcher)
    : contactListener{physicsSystem},
      jobSystem{BuildJobSystem(dispatcher)}
{
    physicsSystem.Init(
        maxBodies,
        numBodyMutexes,
        maxBodyPairs,
        maxContactConstraints,
        layerMap,
        objectVsBroadphaseFilter,
        objectLayerPairFilter
    );

    physicsSystem.SetContactListener(&contactListener);
    ctx = std::make_unique<ComponentContext>(physicsSystem.GetBodyInterfaceNoLock(), shapeFactory);
}
} // namespace nc::physics
