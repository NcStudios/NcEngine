#include "JobSystem.h"

#include "ncengine/task/AsyncDispatcher.h"
#include "ncutility/NcError.h"

#include "Jolt/Jolt.h"
#include "Jolt/Core/FixedSizeFreeList.h"
#include "Jolt/Core/JobSystemWithBarrier.h"
#include "Jolt/Physics/PhysicsSettings.h"

namespace
{
// JobSystem implementation that routes jobs to Taskflow
class TaskRouter : public JPH::JobSystemWithBarrier
{
    public:
        TaskRouter(const nc::task::AsyncDispatcher& dispatcher)
            : JPH::JobSystemWithBarrier{JPH::cMaxPhysicsBarriers},
              m_dispatcher{dispatcher}
        {
            m_freeList.Init(JPH::cMaxPhysicsJobs, JPH::cMaxPhysicsJobs);
        }

        auto GetMaxConcurrency() const -> int override
        {
            return static_cast<int>(m_dispatcher.MaxConcurrency());
        }

        auto CreateJob(const char* name,
                       JPH::ColorArg color,
                       const JobFunction& jobFunction,
                       JPH::uint32 numDependencies = 0) -> JobHandle override
        {
            auto index = 0u;
            for (;;)
            {
                index = m_freeList.ConstructObject(name, color, this, jobFunction, numDependencies);
                if (index != JPH::FixedSizeFreeList<Job>::cInvalidObjectIndex)
                {
                    break;
                }

                NC_ASSERT(false, "Job allocation failed");
                std::this_thread::sleep_for(std::chrono::microseconds(100));
            }

            return JPH::JobHandle{&m_freeList.Get(index)};
        }

    protected:
        void QueueJob(Job* job) override
        {
            m_dispatcher.SilentAsync([job]()
            {
                job->Execute();
                job->Release();
            });
        }

        void QueueJobs(Job** jobs, JPH::uint numJobs) override
        {
            for (auto i = 0u; i < numJobs; ++i)
            {
                auto job = jobs[i];
                m_dispatcher.SilentAsync([job]()
                {
                    job->Execute();
                    job->Release();
                });
            }
        }

        void FreeJob(Job* job) override
        {
            m_freeList.DestructObject(job);
        }

    private:
        nc::task::AsyncDispatcher m_dispatcher;
        JPH::FixedSizeFreeList<Job> m_freeList;
};
} // anonymous namespace

namespace nc::physics
{
auto BuildJobSystem(const task::AsyncDispatcher& dispatcher) -> std::unique_ptr<JPH::JobSystem>
{
    return std::make_unique<TaskRouter>(dispatcher);
}
} // namespace nc::physics
