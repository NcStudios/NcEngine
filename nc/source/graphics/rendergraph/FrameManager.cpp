#include "FrameManager.h"
#include "graphics/d3dresource/GraphicsResourceManager.h"
#include "graphics/d3dresource/GraphicsResource.h"

using namespace nc::graphics::d3dresource;

namespace nc::graphics
{
    void FrameManager::Accept(Job job, size_t targetPass) noexcept
    {
        m_passes[targetPass].Accept(job);
    }

    /** @todo: Replace the body of this function (and eventually the entire class) with a loop
     * where each pass defines the setup/teardown and input/output requirements.
     **/
    void FrameManager::Execute(Graphics* gfx) const
    {
        // PBR Shading Pass
        GraphicsResourceManager::Acquire<Stencil>(Stencil::Mode::Off)->Bind();
        m_passes[0].Execute(gfx);
    }

    void FrameManager::Reset() noexcept
    {
        for (auto& pass : m_passes)
        {
            pass.Reset();
        }
    }

}