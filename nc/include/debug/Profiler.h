#pragma once

#include "platform/SourceLocation.h"

#ifdef NC_EDITOR_ENABLED
    #define NC_PROFILE_BEGIN(filter); \
        static const auto NC_FUNCTION_ID = nc::debug::profiler::Register(NC_SOURCE_FUNCTION, filter); \
        nc::debug::profiler::Push(NC_FUNCTION_ID);
    #define NC_PROFILE_END(); \
        nc::debug::profiler::Pop();
#else
    #define NC_PROFILE_BEGIN(filter);
    #define NC_PROFILE_END();
#endif

namespace nc::debug::profiler
{
    enum class Filter : unsigned
    {
        All = 0u,
        Logic = 1u,
        Collision = 2u,
        Dynamics = 3u,
        Rendering = 4u,
        User = 5u
    };
}

/* Internal Use */
#ifdef NC_EDITOR_ENABLED
#include "time/NcTime.h"
#include <array>
#include <string>
#include <unordered_map>

namespace nc::debug::profiler
{
    struct FunctionMetrics;

    using FuncId = uint32_t;
    using ProfileData = std::unordered_map<FuncId, FunctionMetrics>;

    FuncId Register(const char* name, Filter filter);
    void Push(FuncId id);
    void Pop();
    ProfileData& GetData();
    void UpdateHistory(FunctionMetrics* metrics);
    void Reset(FunctionMetrics* metrics);
    std::tuple<float, float> ComputeAverages(FunctionMetrics const* metrics);
    const char* ToCString(Filter filter);


    struct FunctionMetrics
    {
        FunctionMetrics(std::string name, Filter profileFilter);

        static constexpr unsigned historySize = 64u;
        std::string functionName;
        std::array<float, historySize> callHistory;
        std::array<float, historySize> timeHistory;
        time::Timer timer;
        unsigned callCount;
        float execTime;
        Filter filter;
        bool active;
    };
} // end namespace nc::debug
#endif