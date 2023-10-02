#pragma once

namespace nc
{
/** @brief Base class for non-copyable non-movable types. */
class StableAddress
{
    public:
        StableAddress() noexcept = default;
        StableAddress(const StableAddress&) = delete;
        StableAddress(StableAddress&&) = delete;
        StableAddress& operator=(const StableAddress&) = delete;
        StableAddress& operator=(StableAddress&&) = delete;
    
    protected:
        ~StableAddress() noexcept = default;
};
} // namespace nc
