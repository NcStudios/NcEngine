#pragma once

#include "Pass.h"
#include "graphics2/frontend/subsystem/MeshRendererRenderState.h"

#include "Graphics/GraphicsEngine/interface/DeviceContext.h"

#include <vector>

namespace nc::graphics
{
class MaterialPassBackend
{
    public:
        explicit MaterialPassBackend(std::vector<Pass> passes)
            : m_passes{std::move(passes)}
        {
        }

        void Render(Diligent::IDeviceContext& context, const std::vector<PassRenderState>& passStates);

    private:
        std::vector<Pass> m_passes;
};
} // namespacae nc::graphics
