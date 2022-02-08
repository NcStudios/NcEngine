#pragma once

#include "Component.h"

#include <functional>

namespace nc
{
    class Registry;

    /** Runs custom logic each frame. */
    class FrameLogic final : public ComponentBase
    {
        public:
            using function_t = std::function<void(Entity, Registry*, float)>;

            FrameLogic(Entity entity, function_t func)
                : ComponentBase{entity},
                  m_func{std::move(func)}
            {
            }

            void SetFunction(function_t func)
            {
                m_func = std::move(func);
            }

            void Run(Registry* registry, float dt)
            {
                if(m_func) m_func(ParentEntity(), registry, dt);
            }

        private:
            function_t m_func;
    };

    /** Runs custom logic each physics update tick. */
    class FixedLogic final : public ComponentBase
    {
        public:
            using function_t = std::function<void(Entity, Registry*)>;

            FixedLogic(Entity entity, function_t func)
                : ComponentBase{entity},
                  m_func{std::move(func)}
            {
            }

            void SetFunction(function_t func)
            {
                m_func = std::move(func);
            }

            void Run(Registry* registry)
            {
                if(m_func) m_func(ParentEntity(), registry);
            }

        private:
            function_t m_func;
    };

    /** Runs custom logic on collision and trigger events. */
    class CollisionLogic final : public ComponentBase
    {
        public:
            using function_t = std::function<void(Entity, Entity, Registry*)>;

            CollisionLogic(Entity entity,
                           function_t onCollisionEnter = nullptr,
                           function_t onCollisionExit = nullptr,
                           function_t onTriggerEnter = nullptr,
                           function_t onTriggerExit = nullptr)
                : ComponentBase{entity},
                  m_onCollisionEnter{std::move(onCollisionEnter)},
                  m_onCollisionExit{std::move(onCollisionExit)},
                  m_onTriggerEnter{std::move(onTriggerEnter)},
                  m_onTriggerExit{std::move(onTriggerExit)}
            {
            }

            void SetOnCollisionEnter(function_t func) { m_onCollisionEnter = std::move(func); }
            void SetOnCollisionExit(function_t func) { m_onCollisionExit = std::move(func); }
            void SetOnTriggerEnter(function_t func) { m_onTriggerEnter = std::move(func); }
            void SetOnTriggerExit(function_t func) { m_onTriggerExit = std::move(func); }

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
            function_t m_onCollisionEnter;
            function_t m_onCollisionExit;
            function_t m_onTriggerEnter;
            function_t m_onTriggerExit;
    };
}