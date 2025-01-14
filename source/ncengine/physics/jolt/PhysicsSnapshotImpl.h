#pragma once

#include "ncengine/physics/PhysicsSnapshot.h"

#include "ncjolt/ByteArrayStream.h"

#include "Jolt/Jolt.h"
#include "Jolt/Physics/StateRecorderImpl.h"
#include "Jolt/Physics/PhysicsSystem.h"

namespace nc::physics
{


class PhysicsSnapshotImpl : public PhysicsSnapshot
{
    class SnapshotRecorder : public JPH::StateRecorder
    {
        public:
            void ReadBytes(void* out, size_t numBytes)                       override { m_impl.ReadBytes(out, numBytes); }
            void WriteBytes(const void* data, size_t numBytes)               override { m_impl.WriteBytes(data, numBytes); }
            auto IsEOF()                                       const -> bool override { return m_impl.IsEOF(); }
            auto IsFailed()                                    const -> bool override { return m_impl.IsFailed(); }
            void Reset()                                                              { m_impl.Reset(); }

        private:
            jolt::ByteArrayStream m_impl;
    };

    public:
        explicit PhysicsSnapshotImpl()
        {
        }

        ~PhysicsSnapshotImpl() noexcept
        {
        }

        void Save(JPH::PhysicsSystem& physicsSystem, size_t frame)
        {
            // ensure cleared?...
            physicsSystem.SaveState(m_recorder);
            m_frame = frame;
        }

        auto Restore(JPH::PhysicsSystem& physicsSystem) -> size_t
        {
            physicsSystem.RestoreState(m_recorder);
            m_recorder.Reset();
            return std::exchange(m_frame, NullFrame);
        }

        auto GetRecorder() -> JPH::StateRecorder& { return m_recorder; }

    private:
        SnapshotRecorder m_recorder;
};



} // namespace nc::physics

namespace nc
{
auto MakePhysicsSnapshot() -> std::unique_ptr<PhysicsSnapshot>
{
    return std::make_unique<physics::PhysicsSnapshotImpl>();
}
}
