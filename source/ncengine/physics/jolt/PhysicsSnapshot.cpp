#pragma once

#include "ncengine/physics/PhysicsSnapshot.h"

#include "ncjolt/ByteArrayStream.h"
#include "ncutility/NcError.h"

#include "Jolt/Jolt.h"
#include "Jolt/Physics/StateRecorder.h"
#include "Jolt/Physics/PhysicsSystem.h"

namespace
{
class SnapshotRecorder : public JPH::StateRecorder
{
    public:
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
} // anonymous namespace

namespace nc
{
struct PhysicsSnapshot::Impl
{
    SnapshotRecorder recorder;
};

PhysicsSnapshot::PhysicsSnapshot()
    : m_impl{std::make_unique<PhysicsSnapshot::Impl>()}
{
}

PhysicsSnapshot::PhysicsSnapshot(PhysicsSnapshot&&) noexcept = default;
PhysicsSnapshot& PhysicsSnapshot::operator=(PhysicsSnapshot&&) noexcept = default;
PhysicsSnapshot::~PhysicsSnapshot() noexcept = default;

auto PhysicsSnapshot::GetSize() const -> size_t
{
    return m_impl->recorder.GetSize();
}

auto PhysicsSnapshot::ViewBuffer() const -> std::span<const uint8_t>
{
    return m_impl->recorder.ViewBytes();
}

auto PhysicsSnapshot::ExtractBuffer() -> std::vector<uint8_t>
{
    auto buffer = m_impl->recorder.ExtractBytes();
    Clear();
    return buffer;
}

void PhysicsSnapshot::ResetRead()
{
    m_impl->recorder.ResetRead();
}

void PhysicsSnapshot::Clear()
{
    m_impl->recorder.Reset();
    m_tick = PhysicsTick::Null();
}

void PhysicsSnapshot::SetValidationMode(bool enabled)
{
    m_impl->recorder.SetValidating(enabled);
}

void PhysicsSnapshot::Save(std::any physicsSystem, PhysicsTick tick)
{
    NC_ASSERT(!IsValid(), "PhysicsSnapshot::Clear() must be called before reusing a snapshot.");
    auto* joltPhysics = std::any_cast<JPH::PhysicsSystem*>(physicsSystem);
    joltPhysics->SaveState(m_impl->recorder);
    m_tick = tick;
}

auto PhysicsSnapshot::Restore(std::any physicsSystem) -> bool
{
    auto* joltPhysics = std::any_cast<JPH::PhysicsSystem*>(physicsSystem);
    return joltPhysics->RestoreState(m_impl->recorder);
}
} // namespace nc
