/**
 * @file NcSceneManager.h
 * @copyright Jaremie Romer and McCallister Romer 2024
 */
#pragma once

#include "ncengine/module/ModuleRegistry.h"
#include "ncengine/scene/Scene.h"
#include "ncengine/type/EngineId.h"

#include <functional>

namespace nc
{
/**
 * @brief Module managing the scene queue and transitions.
 * 
 * The simplest and preferred method of changing Scenes is by queueing the desired Scene(s) and calling ScheduleTransition()
 * when it is time to perform the transition. This schedules the transition at the end of the frame, guaranteeing no tasks
 * are in flight. The following routine is performed before starting the next frame:
 *   - Unload() is called on the active Scene.
 *   - The active Scene's destructor is called.
 *   - Clear() is called on all registered Modules.
 *   - ClearSceneData() is called on the ComponentRegistry.
 *   - OnBeforeSceneLoad() is called on each Module.
 *   - Dequeue the next queued Scene and call Load().
*/
class NcScene : public Module
{
    public:
        explicit NcScene() noexcept
            : Module{NcSceneId} {}

        virtual ~NcScene() = default;

        /**
         * @brief Add a scene to the scene queue.
         * @return The added scene's position in the scene queue.
         */
        virtual auto Queue(std::unique_ptr<Scene> scene) noexcept -> size_t = 0;

        /** @brief Remove a scene from the scene queue. */
        virtual void DequeueScene(size_t queuePosition) = 0;

        /** @brief Get the number of scenes in the scene queue. */
        virtual auto GetNumberOfScenesInQueue() const noexcept -> size_t = 0;

        /** @brief Schedule a transition to the next queued scene upon completion of the current frame. */
        virtual void ScheduleTransition() noexcept = 0;

        /** @brief Check if a scene transition is currently scheduled. */
        virtual auto IsTransitionScheduled() const noexcept -> bool = 0;

        /**
         * @brief Unload the active scene and destroy it.
         * @return True if the scene was unloaded or false if there is no active scene.
         * @note Unloading is automatically handled internally when ScheduleTransition() is used.
         */
        virtual auto UnloadActiveScene() -> bool = 0;

        /**
         * @brief Load the next scene from the scene queue.
         * @return True if the scene was loaded or false if no scene's are queued.
         * @throw NcError if there is an active scene loaded.
         * @note Loading is automatically handled internally when ScheduleTransition() is used.
         */
        virtual auto LoadQueuedScene(Registry* registry, ModuleRegistry& modules) -> bool = 0;
};

/** @brief Build an NcScene instance. */
auto BuildSceneModule() -> std::unique_ptr<NcScene>;
} // namespace nc
