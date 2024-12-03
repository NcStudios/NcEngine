#pragma once

#include "MaterialPass.h"
#include "graphics2/frontend/subsystem/MeshRendererRenderState.h"

#include "Graphics/GraphicsEngine/interface/DeviceContext.h"

#include <vector>

namespace nc::graphics
{
class PerPassResourceSignature;

class MaterialPassBackend
{
    public:
        explicit MaterialPassBackend(std::vector<MaterialPass> passes)
            : m_passes{std::move(passes)}
        {
        }

        void Render(Diligent::IDeviceContext& context,
                    Diligent::ISwapChain& swapChain,
                    PerPassResourceSignature& perPassResourceSignature,
                    const std::vector<std::vector<Batch>>& passBatches);

    private:
        std::vector<MaterialPass> m_passes;
};
} // namespacae nc::graphics
