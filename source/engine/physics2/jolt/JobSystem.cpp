#include "JobSystem.h"
#include "Allocator.h"

#include "ncengine/task/AsyncDispatcher.h"

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
        TaskRouter()
            : m_executor{}
        {
            // todo: take in ctor
            JPH::JobSystemWithBarrier::Init(8);
        }

        auto GetMaxConcurrency() const -> int override
        {
            // todo: can't access value b/c of init timing
            return 8;
            // return static_cast<int>(m_executor->num_workers());
        }

        auto CreateJob(const char* name,
                       JPH::ColorArg color,
                       const JobFunction& jobFunction,
                       JPH::uint32 numDependencies = 0) -> JobHandle override
        {
            // todo: hack
            static auto _ = [&]()
            {
                // m_executor = nc::task::Executor::s_executor;
                return true;
            }();

            // todo: allocator?
            auto job = new JPH::JobSystem::Job(name, color, this, jobFunction, numDependencies);
            return JPH::JobSystem::JobHandle{job};
        }

    protected:
        void QueueJob(Job* job) override
        {
            m_executor->silent_async([job]()
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
                m_executor->silent_async([job]()
                {
                    job->Execute();
                    job->Release();
                });
            }
        }

        void FreeJob(Job* job) override
        {
            delete job;
        }

    private:
        tf::Executor* m_executor;
};

class TaskRouterFreeList : public JPH::JobSystemWithBarrier
{
    public:
        TaskRouterFreeList(const nc::task::AsyncDispatcher& dispatcher)
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
                    break;
                JPH_ASSERT(false, "No jobs available!");
                std::this_thread::sleep_for(std::chrono::microseconds(100));
            }

            auto* job = &m_freeList.Get(index);

            // Construct handle to keep a reference, the job is queued below and may immediately complete
            // can just return, right?
            JPH::JobHandle handle(job);

            // // If there are no dependencies, queue the job now
            // if (numDependencies == 0)
            //     QueueJob(job);

            // Return the handle
            return handle;
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
    return std::make_unique<TaskRouterFreeList>(dispatcher);
}
} // namespace nc::physics
