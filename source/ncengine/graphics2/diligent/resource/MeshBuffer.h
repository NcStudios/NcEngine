#pragma once

#include "graphics2/diligent/pass/PassTypes.h"

#include "Common/interface/RefCntAutoPtr.hpp"
#include "Graphics/GraphicsEngine/interface/RenderDevice.h"
#include "Graphics/GraphicsEngine/interface/DeviceContext.h"

#include "ncengine/utility/EnumUtilities.h"

#include <array>
#include <span>
#include <vector>

namespace nc
{
namespace asset
{
struct MeshVertex;
} // namespace asset

namespace graphics
{
auto GetMeshVertexLayoutElements(VertexAttribute attributes,
                                 uint32_t slot = 0u,
                                 uint32_t indexOffset = 0u) -> std::vector<Diligent::LayoutElement>;

class MeshBuffer
{
    public:
        void SetBuffers(Diligent::IDeviceContext& context);

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
