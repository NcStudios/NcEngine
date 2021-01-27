#ifdef NC_EDITOR_ENABLED

#include "Profile.h"

#include <algorithm>
#include <numeric>
#include <stdexcept>
#include <vector>

namespace nc::debug::profile
{
    namespace internal
    {
        FuncId currentId = 0u;
        ProfileData data{};
        std::vector<FuncId> openProfiles{};
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

    void FunctionMetrics::UpdateHistory()
    {
        std::shift_left(callHistory.begin(), callHistory.end(), 1);
        callHistory.back() = static_cast<float>(callCount);

        std::shift_left(timeHistory.begin(), timeHistory.end(), 1);
        timeHistory.back() = execTime / 1000000.0f;
    }

    std::tuple<float, float> FunctionMetrics::GetAverages() const
    {
        auto callSum = std::accumulate(callHistory.begin(), callHistory.end(), 0.0f);
        auto timeSum = std::accumulate(timeHistory.begin(), timeHistory.end(), 0.0f);
        return { callSum / static_cast<float>(historySize),  timeSum / static_cast<float>(historySize) };
    }

    void FunctionMetrics::Reset()
    {
        callCount = 0u;
        execTime = 0.0f;
    }

    FuncId Register(const char* name, Filter filter)
    {
        auto [it, result] = internal::data.emplace(internal::currentId, FunctionMetrics{name, filter});

        if(!result)
            throw std::runtime_error("profile::Register - Failed to emplace FunctionMetrics");

        return internal::currentId++;
    }

    void Push(FuncId id)
    {
        internal::openProfiles.push_back(id);
        auto& data = internal::data.at(id);
        ++data.callCount;
        data.timer.Start();
    }

    void Pop()
    {
        auto mostRecent = internal::openProfiles.back();
        internal::openProfiles.pop_back();
        auto& data = internal::data.at(mostRecent);
        data.timer.Stop();
        data.execTime += data.timer.Value();
    }

    ProfileData& GetData()
    {
        return internal::data;
    }
} // end namespace nc::debug
#endif