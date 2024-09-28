// ExternalProfileMeasurement Implementation (currently MSVC-only)
// see Jolt/Core/Profiler.h
#ifdef JPH_EXTERNAL_PROFILE

#include "Jolt/Jolt.h"
#include "Jolt/Core/Profiler.h"

// For 'real' targets, both jolt and nc profiling will be in sync. This just allows us to build tests when
// we have a profiled build of jolt without bringing in the optick dependency as well.
#ifdef NC_PROFILING_ENABLED
#include "optick.h"
#endif

JPH_NAMESPACE_BEGIN

ExternalProfileMeasurement::ExternalProfileMeasurement([[maybe_unused]] const char* name, uint32)
{
#ifdef NC_PROFILING_ENABLED
    // Optick macros use a static variable to hold a description and a local variable to start/stop. We need to manually
    // build a description based on the given name and emplace an event over our user data to take ownership.
    constexpr auto category = Optick::Category::Physics;
    static const auto color = Optick::Category::GetColor(category);
    static const auto filter = Optick::Category::GetMask(category);
    // note: The use case for CreateShared is to make a copy of the string, which we don't care about, but it also
    // caches the description. Since we aren't caching in a static var, its considerably more performant.
    const auto description = Optick::EventDescription::CreateShared(name, __FILE__, __LINE__, color, filter);
    static_assert(sizeof(Optick::Event) < sizeof(mUserData));
    new (mUserData) Optick::Event(*description);
#endif
}

ExternalProfileMeasurement::~ExternalProfileMeasurement()
{
#ifdef NC_PROFILING_ENABLED
    reinterpret_cast<Optick::Event*>(&mUserData)->~Event();
#endif
}

JPH_NAMESPACE_END
#endif
