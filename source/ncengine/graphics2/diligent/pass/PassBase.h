#pragma once

#include "Common/interface/RefCntAutoPtr.hpp"
#include "Graphics/GraphicsEngine/interface/PipelineState.h"

#include <cstdint>

namespace nc::graphics
{

struct RenderTargets
{
    uint32_t colorIndex;
    uint32_t depthIndex;
};

// PassDispatch needs to iterate through all of the intended material passes, wireframe passes, and post process passes, and populate

class PassDesc
{
    public:
        auto GetSource() const -> const RenderTargets& { return m_source; }
        auto GetSink() const -> const RenderTargets& { return m_sink; }

    private:
        uint32_t m_order;
        RenderTargets m_source;
        RenderTargets m_sink;
};
} // namespace nc::graphics
