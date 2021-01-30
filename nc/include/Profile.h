#pragma once

namespace nc::debug::profile
{
    enum class Filter : unsigned
    {
        All = 0u,
        Engine = 1u,
        User = 2u
    };
}

#ifdef NC_EDITOR_ENABLED
/** @todo use std::source_location when mingw catches up to gcc 11 and add optional override for
* function names (can return same name for different funcs)*/
#define NC_PROFILE_BEGIN(filter); static const auto NC_FUNCTION_ID = nc::debug::profile::Register(__PRETTY_FUNCTION__, filter); \
                                  nc::debug::profile::Push(NC_FUNCTION_ID);
#define NC_PROFILE_END(); nc::debug::profile::Pop();
#else
#define NC_PROFILE_BEGIN(filter);
#define NC_PROFILE_END();
#endif

#ifdef NC_EDITOR_ENABLED
#include "time/NcTime.h"
#include <array>
#include <string>
#include <unordered_map>

namespace nc::debug::profile
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