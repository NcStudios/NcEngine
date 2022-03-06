#pragma once

#include "modules.h"
#include "nc_engine.h"
#include "task/TaskGraph.h"
#include "time/NcTime.h"
#include "window/WindowImpl.h"

namespace nc
{
    class runtime : public nc_engine
    {
        public:
            runtime(engine_init_flags flags);

            void start(std::unique_ptr<Scene> initialScene) override;
            void stop() noexcept override;
            void shutdown() noexcept override;

            auto Audio() noexcept -> nc::audio_module* override;
            auto Environment() noexcept -> nc::Environment* override;
            auto MainCamera() noexcept -> nc::MainCamera* override;
            auto Physics() noexcept -> physics_module* override;
            auto Random() noexcept -> nc::Random* override;
            auto Registry() noexcept -> nc::Registry* override;
            auto SceneSystem() noexcept -> nc::SceneSystem* override;
            auto UI() noexcept -> UISystem* override;

        private:
            window::WindowImpl m_window;
            modules m_modules;
            time::Time m_time;
            tf::Executor m_taskExecutor;
            TaskGraph m_tasks;
            float m_dt;
            float m_dtFactor;
            bool m_isRunning;
            unsigned m_currentPhysicsIterations;

            void build_task_graph();
            void clear();
            void run();

            // remove these
            void do_scene_swap();
            void run_frame_logic();
            void frame_render();
            void frame_cleanup();
    };
}