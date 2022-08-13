#pragma once

#include "Component.h"

#include <functional>

namespace nc
{
class Registry;

using FrameLogicCallable_t = std::function<void(Entity, Registry*, float)>;

template<class Func>
concept FrameLogicCallable = std::convertible_to<Func, FrameLogicCallable_t>;

/** Runs custom logic each frame. */
class FrameLogic final : public ComponentBase
{
    public:
        template<FrameLogicCallable Func>
        FrameLogic(Entity entity, Func&& func)
            : ComponentBase{entity},
                m_func{std::forward<Func>(func)}
        {
        }

        template<FrameLogicCallable Func>
        void SetFunction(Func&& func)
        {
            m_func = std::forward<Func>(func);
        }

        void Run(Registry* registry, float dt)
        {
            if(m_func) m_func(ParentEntity(), registry, dt);
        }

    private:
        FrameLogicCallable_t m_func;
};

using FixedLogicCallable_t = std::function<void(Entity, Registry*)>;

template<class Func>
concept FixedLogicCallable = std::convertible_to<Func, FixedLogicCallable_t>;

/** Runs custom logic each physics update tick. */
class FixedLogic final : public ComponentBase
{
    public:
        template<std::invocable<Entity, Registry*> Func>
        FixedLogic(Entity entity, Func&& func)
            : ComponentBase{entity},
                m_func{std::forward<Func>(func)}
        {
        }

        template<std::invocable<Entity, Registry*> Func>
        void SetFunction(Func&& func)
        {
            m_func = std::forward<Func>(func);
        }

        void Run(Registry* registry)
        {
            if(m_func) m_func(ParentEntity(), registry);
        }

    private:
        FixedLogicCallable_t m_func;
};

using CollisionLogicCallable_t = std::function<void(Entity, Entity, Registry*)>;

template<class Func>
concept CollisionLogicCallable = std::convertible_to<Func, CollisionLogicCallable_t>;

/** Runs custom logic on collision and trigger events. */
class CollisionLogic final : public ComponentBase
{
    public:
        template<CollisionLogicCallable CollisionEnterFunc,
                    CollisionLogicCallable CollisionExitFunc,
                    CollisionLogicCallable TriggerEnterFunc,
                    CollisionLogicCallable TriggerExitFunc>
        CollisionLogic(Entity entity,
                        CollisionEnterFunc&& onCollisionEnter = nullptr,
                        CollisionExitFunc&& onCollisionExit = nullptr,
                        TriggerEnterFunc&& onTriggerEnter = nullptr,
                        TriggerExitFunc&& onTriggerExit = nullptr)
            : ComponentBase{entity},
                m_onCollisionEnter{std::forward<CollisionEnterFunc>(onCollisionEnter)},
                m_onCollisionExit{std::forward<CollisionExitFunc>(onCollisionExit)},
                m_onTriggerEnter{std::forward<TriggerEnterFunc>(onTriggerEnter)},
                m_onTriggerExit{std::forward<TriggerExitFunc>(onTriggerExit)}
        {
        }

        template<CollisionLogicCallable Func>
        void SetOnCollisionEnter(Func&& func)
        {
            m_onCollisionEnter = std::forward<Func>(func);
        }

        template<CollisionLogicCallable Func>
        void SetOnCollisionExit(Func&& func)
        {
            m_onCollisionExit = std::forward<Func>(func);
        }

        template<CollisionLogicCallable Func>
        void SetOnTriggerEnter(Func&& func)
        {
            m_onTriggerEnter = std::forward<Func>(func);
        }

        template<CollisionLogicCallable Func>
        void SetOnTriggerExit(Func&& func)
        {
            m_onTriggerExit = std::forward<Func>(func);
        }

        void NotifyCollisionEnter(Entity other, Registry* registry)
        {
            if(m_onCollisionEnter) m_onCollisionEnter(ParentEntity(), other, registry);
        }

        void NotifyCollisionExit(Entity other, Registry* registry)
        {
            if(m_onCollisionExit) m_onCollisionExit(ParentEntity(), other, registry);
        }

        void NotifyTriggerEnter(Entity other, Registry* registry)
        {
            if(m_onTriggerEnter) m_onTriggerEnter(ParentEntity(), other, registry);
        }

        void NotifyTriggerExit(Entity other, Registry* registry)
        {
            if(m_onTriggerExit) m_onTriggerExit(ParentEntity(), other, registry);
        }

    private:
        CollisionLogicCallable_t m_onCollisionEnter;
        CollisionLogicCallable_t m_onCollisionExit;
        CollisionLogicCallable_t m_onTriggerEnter;
        CollisionLogicCallable_t m_onTriggerExit;
};
} // namespace nc