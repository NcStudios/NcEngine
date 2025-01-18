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
    static constexpr auto NullFrame = std::numeric_limits<size_t>::max();

    public:
        explicit PhysicsSnapshotImpl(bool enableValidation)
        {
            if (enableValidation)
            {
                m_recorder.SetValidating(true);
            }
        }

        auto IsValid()  const -> bool   { return m_frame != NullFrame; }
        auto GetFrame() const -> size_t { return m_frame; }
        auto GetSize()  const -> size_t { return m_recorder.GetSize(); }

        void Save(JPH::PhysicsSystem& physicsSystem, size_t frame)
        {
            NC_ASSERT(!IsValid(), "PhysicsSnapshot::Clear() must be called before reusing a snapshot.");
            physicsSystem.SaveState(m_recorder);
            m_frame = frame;
        }

        auto Restore(JPH::PhysicsSystem& physicsSystem) -> bool
        {
            const auto restored = physicsSystem.RestoreState(m_recorder);
            m_recorder.Reset();
            m_frame = NullFrame;
            return restored;
        }

        void ResetRead()
        {
            m_recorder.ResetRead();
        }

        void Clear()
        {
            m_recorder.Reset();
            m_frame = NullFrame;
        }

        void SetValidationMode(bool enabled)
        {
            m_recorder.SetValidating(enabled);
        }

    private:
        SnapshotRecorder m_recorder;
        size_t m_frame = NullFrame;
};
} // namespace physics

PhysicsSnapshot::PhysicsSnapshot(bool enableValidation)
    : m_impl{std::make_unique<physics::PhysicsSnapshotImpl>(enableValidation)}
{
}

PhysicsSnapshot::PhysicsSnapshot(PhysicsSnapshot&&) noexcept = default;
PhysicsSnapshot& PhysicsSnapshot::operator=(PhysicsSnapshot&&) noexcept = default;
PhysicsSnapshot::~PhysicsSnapshot() noexcept = default;

auto PhysicsSnapshot::IsValid() const -> bool
{
    return m_impl->IsValid();
}

auto PhysicsSnapshot::GetFrame() const -> size_t
{
    return m_impl->GetFrame();
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
