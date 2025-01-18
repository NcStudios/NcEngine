#pragma once

#include "ncengine/physics/PhysicsSnapshot.h"

#include "ncjolt/ByteArrayStream.h"

#include "Jolt/Jolt.h"
#include "Jolt/Physics/StateRecorderImpl.h"
#include "Jolt/Physics/PhysicsSystem.h"

#include <limits>

namespace
{
class SnapshotRecorder : public JPH::StateRecorder
{
    public:
        void ReadBytes(void* out, size_t numBytes)                       override { m_impl.ReadBytes(out, numBytes); }
        void WriteBytes(const void* in, size_t numBytes)                 override { m_impl.WriteBytes(in, numBytes); }
        auto IsEOF()                                     const -> bool   override { return m_impl.IsEOF(); }
        auto IsFailed()                                  const -> bool   override { return m_impl.IsFailed(); }
        auto GetSize()                                   const -> size_t          { return m_impl.GetBuffer().size(); }
        void Reset()                                                              { m_impl.Reset(); }
        void ResetRead()                                                          { m_impl.ResetRead(); }

    private:
        nc::jolt::ByteArrayStream m_impl;
};

// todo filter

} // anonymous namespace

namespace nc
{
namespace physics
{
class PhysicsSnapshotImpl
{
    public:
        auto IsValid() const -> bool        { return !m_tick.IsNull(); }
        auto GetTick() const -> PhysicsTick { return m_tick; }
        auto GetSize() const -> size_t      { return m_recorder.GetSize(); }

        void Save(JPH::PhysicsSystem& physicsSystem, PhysicsTick tick)
        {
            NC_ASSERT(!IsValid(), "PhysicsSnapshot::Clear() must be called before reusing a snapshot.");
            physicsSystem.SaveState(m_recorder);
            m_tick = tick;
        }

        auto Restore(JPH::PhysicsSystem& physicsSystem) -> bool
        {
            const auto restored = physicsSystem.RestoreState(m_recorder);
            m_recorder.Reset();
            m_tick = PhysicsTick::Null();
            return restored;
        }

        void ResetRead()
        {
            m_recorder.ResetRead();
        }

        void Clear()
        {
            m_recorder.Reset();
            m_tick = PhysicsTick::Null();
        }

        void SetValidationMode(bool enabled)
        {
            m_recorder.SetValidating(enabled);
        }

    private:
        SnapshotRecorder m_recorder;
        PhysicsTick m_tick = PhysicsTick::Null();
};
} // namespace physics

PhysicsSnapshot::PhysicsSnapshot()
    : m_impl{std::make_unique<physics::PhysicsSnapshotImpl>()}
{
}

PhysicsSnapshot::PhysicsSnapshot(PhysicsSnapshot&&) noexcept = default;
PhysicsSnapshot& PhysicsSnapshot::operator=(PhysicsSnapshot&&) noexcept = default;
PhysicsSnapshot::~PhysicsSnapshot() noexcept = default;

auto PhysicsSnapshot::IsValid() const -> bool
{
    return m_impl->IsValid();
}

auto PhysicsSnapshot::GetTick() const -> PhysicsTick
{
    return m_impl->GetTick();
}

auto PhysicsSnapshot::GetSize() const -> size_t
{
    return m_impl->GetSize();
}

auto PhysicsSnapshot::GetImpl() -> physics::PhysicsSnapshotImpl&
{
    return *m_impl;
}

void PhysicsSnapshot::ResetRead()
{
    m_impl->ResetRead();
}

void PhysicsSnapshot::Clear()
{
    m_impl->Clear();
}

void PhysicsSnapshot::SetValidationMode(bool enabled)
{
    m_impl->SetValidationMode(enabled);
}
} // namespace nc
