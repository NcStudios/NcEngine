#include "TransformBuffer.h"
#include "component/Transform.h"
#include "graphics/Graphics2.h"
#include "graphics/d3dresource/GraphicsResourceManager.h" // @todo: not this

namespace nc::graphics::vulkan
{
    TransformMatrices GetMatrices(DirectX::XMMATRIX modelView, DirectX::XMMATRIX modelViewProjection)
    {
        return
        {
            DirectX::XMMatrixTranspose(modelView),
            DirectX::XMMatrixTranspose(modelViewProjection)
        };
    }
}