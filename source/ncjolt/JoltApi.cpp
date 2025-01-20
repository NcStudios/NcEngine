#include "ncjolt/JoltApi.h"
#include "ncjolt/Allocator.h"

#include "Jolt/Jolt.h"
#include "Jolt/RegisterTypes.h"
#include "Jolt/Core/Factory.h"
#include "Jolt/Core/IssueReporting.h"

namespace
{
auto DefaultAssertCB(const char*, const char*, const char*, unsigned) -> bool
{
    return true;
};
} // anonymous namespace

namespace nc::jolt
{
JoltApi::JoltApi(AssertFailedCallback assertCB)
{
    JPH::AssertFailed = assertCB ? assertCB : ::DefaultAssertCB;
    RegisterAllocator();
    m_factory = std::make_unique<JPH::Factory>();
    JPH::Factory::sInstance = m_factory.get();
    JPH::RegisterTypes();
}

JoltApi::~JoltApi() noexcept
{
    JPH::UnregisterTypes();
    JPH::Factory::sInstance = nullptr;
    JPH::AssertFailed = ::DefaultAssertCB;
}
} // namespace nc::jolt
