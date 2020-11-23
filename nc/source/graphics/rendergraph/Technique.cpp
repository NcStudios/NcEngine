#include "Technique.h"
#include "graphics/Model.h"

namespace nc::graphics
{
    void Technique::Submit(FrameManager& frame, const nc::graphics::Model& model) const noexcept
    {
        if (!m_isEnabled)
        {
            return;
        }

        for (const auto& step : m_steps)
        {
            step.Submit(frame, model);
        }
    }

    void Technique::AddStep(Step step) noexcept
    {
        m_steps.push_back(std::move(step));
    }

    void Technique::Enable(bool isEnabled)
    {
        m_isEnabled = isEnabled;
    }
}