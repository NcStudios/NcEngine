/**
 * @file FrameLogic.h
 * @copyright Jaremie Romer and McCallister Romer 2024
 */
#pragma once

#include "ncengine/ecs/Component.h"
#include "ncengine/ecs/Ecs.h"

#include <functional>

namespace nc
{
/** @brief FrameLogic callable member type */
using FrameLogicCallable_t = std::function<void(Entity self, ecs::Ecs world, float dt)>;

/** @brief FrameLogic callable type requirements */
template<class Func>
concept FrameLogicCallable = std::convertible_to<Func, FrameLogicCallable_t>;

/** @brief Component that runs a custom callable during each logic phase. */
class FrameLogic final : public ComponentBase
{
    public:
        template<FrameLogicCallable Func>
        FrameLogic(Entity entity, Func&& func)
            : ComponentBase{entity},
              m_func{std::forward<Func>(func)}
        {
        }

        /** @brief Set a new callable. */
        template<FrameLogicCallable Func>
        void SetFunction(Func&& func)
        {
            m_func = std::forward<Func>(func);
        }

        void Run(ecs::Ecs world, float dt)
        {
            if(m_func) m_func(ParentEntity(), world, dt);
        }

    private:
        FrameLogicCallable_t m_func;
};
} // namespace nc
