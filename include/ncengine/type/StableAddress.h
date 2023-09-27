#pragma once

namespace nc
{
/**
 * @brief Base class for non-copyable non-movable types. Additionally, all types
 *        derived from StableAddress are guaranteed to exist for the lifetime of
 *        the NcEngine instance.
 */
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
