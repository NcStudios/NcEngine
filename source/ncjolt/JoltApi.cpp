#include "ncjolt/JoltApi.h"
#include "ncjolt/Allocator.h"

#include "Jolt/Jolt.h"
#include "Jolt/RegisterTypes.h"
#include "Jolt/Core/Factory.h"

namespace nc::jolt
{
JoltApi::JoltApi()
{
    RegisterAllocator();
    m_factory = std::make_unique<JPH::Factory>();
    JPH::Factory::sInstance = m_factory.get();
    JPH::RegisterTypes();
}

JoltApi::~JoltApi() noexcept
{
    JPH::UnregisterTypes();
    JPH::Factory::sInstance = nullptr;
}
} // namespace nc::jolt
