#pragma once

#include "PassTypes.h"

#include "Common/interface/RefCntAutoPtr.hpp"
#include "Graphics/GraphicsEngine/interface/RenderDevice.h"
#include "Graphics/GraphicsEngine/interface/DeviceContext.h"

namespace nc::graphics
{
struct Pass
{
    explicit Pass(Diligent::RefCntAutoPtr<Diligent::IPipelineState> pso_,
                  Sinks sinks_,
                  Sources sources_);
    Diligent::RefCntAutoPtr<Diligent::IPipelineState> pso;
    Sinks sinks;
    Sources sources;
};
}