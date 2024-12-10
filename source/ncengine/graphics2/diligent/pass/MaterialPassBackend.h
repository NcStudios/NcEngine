#pragma once

#include "Pass.h"
#include "graphics2/frontend/subsystem/MeshRenderState.h"

#include "Graphics/GraphicsEngine/interface/DeviceContext.h"

#include <vector>

namespace nc::graphics
{
class MaterialPassBackend
{
    public:
        explicit MaterialPassBackend(std::vector<Pass> staticMeshPasses, std::vector<Pass> skinnedMeshPasses)
            : m_staticMeshPasses{std::move(staticMeshPasses)},
              m_skinnedMeshPasses{std::move(skinnedMeshPasses)}
        {
        }

        void Render(Diligent::IDeviceContext& context,
                    const std::vector<std::vector<Batch>>& staticMeshBatches,
                    const std::vector<std::vector<Batch>>& skinnedMeshBatches);

    private:
        std::vector<Pass> m_staticMeshPasses;
        std::vector<Pass> m_skinnedMeshPasses;
};
} // namespacae nc::graphics
