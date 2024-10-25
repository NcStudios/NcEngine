#pragma once

#include "Common/interface/RefCntAutoPtr.hpp"
#include "Graphics/GraphicsEngine/interface/RenderDevice.h"
#include "Graphics/GraphicsEngine/interface/DeviceContext.h"

#include <span>

namespace nc
{
namespace asset
{
struct MeshVertex;
} // namespace asset

namespace graphics
{
class MeshBuffer
{
    public:
        explicit MeshBuffer()
        {
        }

        // void SetBuffers(Diligent::IDeviceContext& context);

        void Load(std::span<const asset::MeshVertex> vertices,
                  std::span<const uint32_t> indices,
                  Diligent::IDeviceContext& context,
                  Diligent::IRenderDevice& device);

    private:
        Diligent::RefCntAutoPtr<Diligent::IBuffer> m_vertexBuffer;
        Diligent::RefCntAutoPtr<Diligent::IBuffer> m_indexBuffer;
};
} // namespace graphics
} // namespace nc
