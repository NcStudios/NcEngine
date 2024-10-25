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
auto GetMeshVertexLayoutElements(uint32_t slot = 0u, uint32_t indexOffset = 0u) -> std::array<Diligent::LayoutElement, 7>;

class MeshBuffer
{
    public:
        explicit MeshBuffer()
        {
        }

        // todo: add to issue about splitting:
        //       instead of split, could maybe compress data, see forum post

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
