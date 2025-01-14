#pragma once

#include <limits>
#include <memory>

namespace nc
{
class PhysicsSnapshot
{
    public:
        static constexpr auto NullFrame = std::numeric_limits<size_t>::max();

        PhysicsSnapshot(PhysicsSnapshot&&) = delete;
        PhysicsSnapshot& operator=(PhysicsSnapshot&&) = delete;
        PhysicsSnapshot(const PhysicsSnapshot&) = delete;
        PhysicsSnapshot& operator=(const PhysicsSnapshot&) = delete;

        virtual ~PhysicsSnapshot() noexcept = default;


        auto IsValid()  const -> bool   { return m_frame == NullFrame; }
        auto GetFrame() const -> size_t { return m_frame; }

    protected:
        size_t m_frame = NullFrame;

        PhysicsSnapshot() = default;
};

auto MakePhysicsSnapshot() -> std::unique_ptr<PhysicsSnapshot>;
} // namespace nc
