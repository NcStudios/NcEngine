#include "Step.h"
#include "graphics/d3dresource/GraphicsResource.h"
#include "graphics/Graphics.h"
#include "graphics/Model.h"
#include "FrameManager.h"

namespace nc::graphics
{
    Step::Step(size_t targetPass)
    : m_targetPass{targetPass}
    {}

    void Step::Submit(FrameManager& frame, const nc::graphics::Model& model) const noexcept
    {
        frame.Accept( Job{this, &model}, m_targetPass);
    }
}