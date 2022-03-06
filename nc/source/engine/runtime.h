#pragma once

#include "context.h"
#include "modules.h"
#include "nc_engine.h"
#include "task/TaskGraph.h"
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

            auto Audio() noexcept -> audio_module* override;
            auto Graphics() noexcept -> graphics_module* override;
            auto Physics() noexcept -> physics_module* override;
            auto Random() noexcept -> nc::Random* override;
            auto Registry() noexcept -> nc::Registry* override;
            auto SceneSystem() noexcept -> nc::SceneSystem* override;

        private:
            window::WindowImpl m_window;
            context m_context;
            modules m_modules;
            tf::Executor m_taskExecutor;
            TaskGraph m_tasks;
            float m_dt;
            float m_dtFactor;
            bool m_isRunning;
            unsigned m_currentPhysicsIterations; /** @todo should go in physics_module_impl */

            void build_task_graph(); /** @todo could maybe have graph builder class/func */
            void clear();
            void run();

            // remove these
            void do_scene_swap();
            void run_frame_logic();
            void frame_cleanup();
    };
}