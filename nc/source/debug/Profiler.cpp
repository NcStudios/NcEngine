#ifdef NC_EDITOR_ENABLED

#include "debug/Profiler.h"

#include <algorithm>
#include <numeric>
#include <stdexcept>
#include <vector>

namespace nc::debug::profiler
{
    namespace internal
    {
        FuncId currentId = 0u;
        ProfileData data{};
        std::vector<FuncId> openProfiles{};
    }

    const char* ToCString(Filter filter)
    {
        switch(filter)
        {
            case Filter::All:
                return "All";
            case Filter::Logic:
                return "Logic";
            case Filter::Physics:
                return "Physics";
            case Filter::Rendering:
                return "Rendering";
            case Filter::User:
                return "User";
            default:
                throw std::runtime_error("ToCString - Unkown Filter value");
        }
    }

    FunctionMetrics::FunctionMetrics(std::string name, Filter profileFilter)
        : functionName{std::move(name)},
          callHistory{historySize},
          timeHistory{historySize},
          timer{},
          callCount{0u},
          execTime{0.0f},
          filter{profileFilter},
          active{false}
    {
    }

    void UpdateHistory(FunctionMetrics* metrics)
    {
        std::shift_left(metrics->callHistory.begin(), metrics->callHistory.end(), 1);
        metrics->callHistory.back() = static_cast<float>(metrics->callCount);

        std::shift_left(metrics->timeHistory.begin(), metrics->timeHistory.end(), 1);
        metrics->timeHistory.back() = metrics->execTime / 1000000.0f;
    }

    void Reset(FunctionMetrics* metrics)
    {
        metrics->callCount = 0u;
        metrics->execTime = 0.0f;
    }

    std::tuple<float, float> ComputeAverages(FunctionMetrics const* metrics)
    {
        auto callSum = std::accumulate(metrics->callHistory.begin(), metrics->callHistory.end(), 0.0f);
        auto timeSum = std::accumulate(metrics->timeHistory.begin(), metrics->timeHistory.end(), 0.0f);
        const auto historySize = static_cast<float>(metrics->historySize);
        return { callSum / historySize,  timeSum / historySize };
    }

    FuncId Register(const char* name, Filter filter)
    {
        auto [it, result] = internal::data.emplace(internal::currentId, FunctionMetrics{name, filter});

        if(!result)
            throw std::runtime_error("profiler::Register - Failed to emplace FunctionMetrics");

        return internal::currentId++;
    }

    void Push(FuncId id)
    {
        internal::openProfiles.push_back(id);
        try
        {
            auto& data = internal::data.at(id);
            ++data.callCount;
            data.timer.Start();
        }
        catch(const std::exception& e)
        {
            std::throw_with_nested(std::runtime_error("profiler::Push - invalid FuncId"));
        }
    }

    void Pop()
    {
        if(internal::openProfiles.empty())
            throw std::runtime_error("profiler::Pop - Push/Pop mismatch");

        auto mostRecent = internal::openProfiles.back();
        internal::openProfiles.pop_back();
        try
        {
            auto& data = internal::data.at(mostRecent);
            data.timer.Stop();
            data.execTime += data.timer.Value();
        }
        catch(const std::exception& e)
        {
            std::throw_with_nested(std::runtime_error("profiler::Pop - invalid FuncId"));
        }
    }

    ProfileData& GetData()
    {
        return internal::data;
    }
} // end namespace nc::debug
#endif