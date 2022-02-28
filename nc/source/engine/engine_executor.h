#pragma once

#include "engine_context.h"
#include "task/TaskGraph.h"

namespace nc
{
    class NcEngine;

    class engine_executor
    {
        public:
            engine_executor(engine_context context, NcEngine* engine);

            void start(std::unique_ptr<Scene> initialScene);
            void stop() noexcept;
            void shutdown() noexcept;
            auto get_context() noexcept -> engine_context* { return &m_context; }

        private:
            engine_context m_context;
            tf::Executor m_taskExecutor;
            TaskGraph m_tasks;
            NcEngine* m_engine;
            float m_dt;
            float m_frameDeltaTimeFactor;
            size_t m_currentPhysicsIterations;
            bool m_isRunning;

            void game_loop();
            void build_task_graph();
            void clear();
            void do_scene_swap();
            void run_frame_logic(float dt);
            void frame_render();
            void frame_cleanup();
    };
}