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

#ifndef NC_EDITOR_ENABLED
#define NC_PROFILE_BEGIN(filter);
#define NC_PROFILE_END();
#else
#include "time/NcTime.h"
#include <array>
#include <experimental/source_location>
#include <string>
#include <unordered_map>

#define NC_PROFILE_BEGIN(filter); \
    static constexpr auto NC_FUNCTION_NAME = std::experimental::fundamentals_v2::source_location::current().function_name(); \
    static const auto NC_FUNCTION_ID = nc::debug::profile::Register(NC_FUNCTION_NAME, filter); \
    nc::debug::profile::Push(NC_FUNCTION_ID);

#define NC_PROFILE_END(); nc::debug::profile::Pop();

namespace nc::debug::profile
{
    struct FunctionMetrics;

    using FuncId = uint32_t;
    using ProfileData = std::unordered_map<FuncId, FunctionMetrics>;

    FuncId Register(const char* name, Filter filter);
    void Push(FuncId id);
    void Pop();
    ProfileData& GetData();

    struct FunctionMetrics
    {
        FunctionMetrics(std::string name, Filter profileFilter);

        void UpdateHistory();
        std::tuple<float, float> GetAverages() const;
        void Reset();

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