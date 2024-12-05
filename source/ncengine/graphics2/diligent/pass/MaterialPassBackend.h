#pragma once

#include "MaterialPass.h"
#include "graphics2/frontend/subsystem/MeshRendererRenderState.h"

#include "Graphics/GraphicsEngine/interface/DeviceContext.h"

#include <utility>
#include <vector>

namespace nc::graphics
{
class PerPassResourceSignature;

class MaterialPassBackend
{
    public:
        explicit MaterialPassBackend(std::vector<MaterialPass> passes)
            : m_passes{std::move(passes)},
              m_lastColorRenderTargetIndex{0u},
              m_lastDepthRenderTargetIndex{0u}
        {
        }

        void Render(Diligent::IDeviceContext& context,
                    Diligent::ISwapChain& swapChain,
                    PerPassResourceSignature& perPassResourceSignature,
                    const std::vector<std::vector<Batch>>& passBatches);
        
        auto GetLastRenderTargetIndices() -> std::pair<uint32_t, uint32_t> {return std::make_pair(m_lastColorRenderTargetIndex, m_lastDepthRenderTargetIndex);}

    private:
        std::vector<MaterialPass> m_passes;
        uint32_t m_lastColorRenderTargetIndex;
        uint32_t m_lastDepthRenderTargetIndex;
};
} // namespacae nc::graphics
