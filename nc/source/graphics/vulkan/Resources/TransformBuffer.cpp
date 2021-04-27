#include "TransformBuffer.h"
#include "component/Transform.h"
#include "graphics/Graphics2.h"
#include "graphics/d3dresource/GraphicsResourceManager.h" // @todo: not this

namespace nc::graphics::vulkan
{
    TransformMatrices GetMatrices(Transform* transform)
    {
        const auto gfx = nc::graphics::d3dresource::GraphicsResourceManager::GetGraphics2();
        const auto modelView = transform->GetTransformationMatrix() * gfx->GetViewMatrix();
        return
        {
            DirectX::XMMatrixTranspose(modelView),
            DirectX::XMMatrixTranspose(modelView * gfx->GetProjectionMatrix())
        };
    }
}