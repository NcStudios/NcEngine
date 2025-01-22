#include "ncengine/physics/PhysicsSnapshot.h"

#include "ncjolt/ByteArrayStream.h"

#include "Jolt/Jolt.h"
#include "Jolt/Physics/StateRecorder.h"
#include "Jolt/Physics/PhysicsSystem.h"

namespace
{
class SnapshotFilter : public JPH::StateRecorderFilter
{
    public:
        auto ShouldSaveBody(const JPH::Body& body) const -> bool override
        {
            return body.GetMotionType() != JPH::EMotionType::Static;
        }
};

const auto g_filter = SnapshotFilter{};
} // anonymous namespace

namespace nc
{
class PhysicsSnapshot::Impl : public JPH::StateRecorder
{
    public:
        Impl() = default;
        Impl(std::vector<uint8_t> bytes)
            : m_impl{std::move(bytes)}
        {
        }

        void ReadBytes(void* out, size_t count)       override { m_impl.ReadBytes(out, count); }
        void WriteBytes(const void* in, size_t count) override { m_impl.WriteBytes(in, count); }

        auto ViewBytes()    const -> std::span<const uint8_t> { return m_impl.GetBuffer(); }
        auto ExtractBytes()       -> std::vector<uint8_t>     { return m_impl.ExtractBuffer(); }

        auto IsEOF()     const -> bool   override { return m_impl.IsEOF(); }
        auto IsFailed()  const -> bool   override { return m_impl.IsFailed(); }
        auto GetSize()   const -> size_t          { return m_impl.GetBuffer().size(); }
        void Reset()                              { m_impl.Reset(); }
        void ResetRead()                          { m_impl.ResetRead(); }

    private:
        nc::jolt::ByteArrayStream m_impl;
};

PhysicsSnapshot::PhysicsSnapshot()
    : m_impl{std::make_unique<PhysicsSnapshot::Impl>()}
{
}

PhysicsSnapshot::PhysicsSnapshot(PhysicsTick tick, std::vector<uint8_t> bytes)
    : m_impl{std::make_unique<PhysicsSnapshot::Impl>(std::move(bytes))},
      m_tick{tick}
{
}

PhysicsSnapshot::PhysicsSnapshot(PhysicsSnapshot&&) noexcept = default;
PhysicsSnapshot& PhysicsSnapshot::operator=(PhysicsSnapshot&&) noexcept = default;
PhysicsSnapshot::~PhysicsSnapshot() noexcept = default;

auto PhysicsSnapshot::GetSize() const -> size_t
{
    return m_impl->GetSize();
}

auto PhysicsSnapshot::ViewBuffer() const -> std::span<const uint8_t>
{
    return m_impl->ViewBytes();
}

auto PhysicsSnapshot::ExtractBuffer() -> std::vector<uint8_t>
{
    auto buffer = m_impl->ExtractBytes();
    Clear();
    return buffer;
}

void PhysicsSnapshot::ResetRead()
{
    m_impl->ResetRead();
}

void PhysicsSnapshot::Clear()
{
    m_impl->Reset();
    m_tick = PhysicsTick::Null();
}

void PhysicsSnapshot::SetValidationMode(bool enabled)
{
    m_impl->SetValidating(enabled);
}

void PhysicsSnapshot::Save(std::any physicsSystem, PhysicsTick tick)
{
    if (IsValid())
    {
        Clear();
    }

    auto* joltPhysics = std::any_cast<JPH::PhysicsSystem*>(physicsSystem);
    joltPhysics->SaveState(*m_impl, JPH::EStateRecorderState::All, &g_filter);
    m_tick = tick;
}

auto PhysicsSnapshot::Restore(std::any physicsSystem) -> bool
{
    auto* joltPhysics = std::any_cast<JPH::PhysicsSystem*>(physicsSystem);
    return joltPhysics->RestoreState(*m_impl);
}
} // namespace nc
