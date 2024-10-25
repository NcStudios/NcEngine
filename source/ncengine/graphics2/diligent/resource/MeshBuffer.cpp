#include "MeshBuffer.h"
#include "ncengine/asset/AssetData.h"

#include <array>

namespace
{
auto MakeVertexBufferDesc(std::span<const nc::asset::MeshVertex> vertices) -> Diligent::BufferDesc
{
    return Diligent::BufferDesc{
        "VertexBuffer",
        static_cast<uint32_t>(sizeof(nc::asset::MeshVertex) * vertices.size()),
        Diligent::BIND_VERTEX_BUFFER,
        Diligent::USAGE_IMMUTABLE
    };
}

auto MakeIndexBufferDesc(std::span<const uint32_t> indices) -> Diligent::BufferDesc
{
    return Diligent::BufferDesc{
        "IndexBuffer",
        static_cast<uint32_t>(sizeof(uint32_t) * indices.size()),
        Diligent::BIND_INDEX_BUFFER,
        Diligent::USAGE_IMMUTABLE
    };
}
} // anonymous namespace

namespace nc::graphics
{
// todo: can we just call once
// void MeshBuffer::SetBuffers(Diligent::IDeviceContext& context)
// {
//     // note: References to buffers are held by the context. SET_VERTEX_BUFFER_FLAG_RESET is needed here to release
//     //       potentially stale buffers.

//     // todo: What if null/nothing loaded?

//     auto vBuff = m_vertexBuffer.RawPtr();
//     context.SetVertexBuffers(
//         0,
//         1,
//         &vBuff,
//         nullptr,
//         Diligent::RESOURCE_STATE_TRANSITION_MODE_TRANSITION,
//         Diligent::SET_VERTEX_BUFFERS_FLAG_RESET
//     );

//     context.SetIndexBuffer(
//         m_indexBuffer,
//         0,
//         Diligent::RESOURCE_STATE_TRANSITION_MODE_TRANSITION
//     );
// }

void MeshBuffer::Load(std::span<const asset::MeshVertex> vertices,
                      std::span<const uint32_t> indices,
                      Diligent::IDeviceContext& context,
                      Diligent::IRenderDevice& device)
{
    m_vertexBuffer.Release();
    const auto vertexDesc = MakeVertexBufferDesc(vertices);
    const auto vertexData = Diligent::BufferData{vertices.data(), vertexDesc.Size};
    device.CreateBuffer(vertexDesc, &vertexData, &m_vertexBuffer);

    m_indexBuffer.Release();
    const auto indexDesc = MakeIndexBufferDesc(indices);
    const auto indexData = Diligent::BufferData{indices.data(), indexDesc.Size};
    device.CreateBuffer(indexDesc, &indexData, &m_indexBuffer);

    const auto barriers = std::array{
        Diligent::StateTransitionDesc(
            m_vertexBuffer.RawPtr(),
            Diligent::RESOURCE_STATE_UNKNOWN,
            Diligent::RESOURCE_STATE_VERTEX_BUFFER,
            Diligent::STATE_TRANSITION_FLAG_UPDATE_STATE
        ),
        Diligent::StateTransitionDesc(
            m_indexBuffer.RawPtr(),
            Diligent::RESOURCE_STATE_UNKNOWN,
            Diligent::RESOURCE_STATE_INDEX_BUFFER,
            Diligent::STATE_TRANSITION_FLAG_UPDATE_STATE
        )
    };

    context.TransitionResourceStates(static_cast<uint32_t>(barriers.size()), barriers.data());

    // note: References to buffers are also held by the context. SET_VERTEX_BUFFER_FLAG_RESET is
    //       needed here to release potentially stale buffers.
    auto vBuff = m_vertexBuffer.RawPtr();
    context.SetVertexBuffers(
        0,
        1,
        &vBuff,
        nullptr,
        Diligent::RESOURCE_STATE_TRANSITION_MODE_TRANSITION,
        Diligent::SET_VERTEX_BUFFERS_FLAG_RESET
    );

    context.SetIndexBuffer(
        m_indexBuffer,
        0,
        Diligent::RESOURCE_STATE_TRANSITION_MODE_TRANSITION
    );
}
} // namespace nc::graphics
