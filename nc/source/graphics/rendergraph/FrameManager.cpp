#include "FrameManager.h"
#include "graphics/d3dresource/GraphicsResourceManager.h"
#include "graphics/d3dresource/GraphicsResource.h"
#include "graphics/techniques/PhongShadingTechnique.h"
#include "graphics/techniques/WireframeTechnique.h"
#include "debug/Profiler.h"

using namespace nc::graphics::d3dresource;

namespace nc::graphics
{
    void FrameManager::Accept(size_t targetPass, Job job) noexcept
    {
        m_passes[targetPass].Accept(job);
    }

    /** @todo: Replace the body of this function (and eventually the entire class) with a loop
     * where each pass defines the setup/teardown and input/output requirements.
     **/
    void FrameManager::Execute(Graphics* gfx) const
    {
        NC_PROFILE_BEGIN(debug::profiler::Filter::Rendering);
        // PBR Shading Pass
        PhongShadingTechnique::BindCommonResources();
        m_passes[0].Execute(gfx);

        // Wireframe Pass
        WireframeTechnique::BindCommonResources();
        m_passes[1].Execute(gfx);
        NC_PROFILE_END();
    }

    void FrameManager::Reset() noexcept
    {
        for (auto& pass : m_passes)
        {
            pass.Reset();
        }
    }

}