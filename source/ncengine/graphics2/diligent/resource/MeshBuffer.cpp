#include "MeshBuffer.h"
#include "ncengine/asset/AssetData.h"

#include "ncutility/NcError.h"

#include <array>

namespace
{
constexpr auto g_vertexStride = static_cast<uint32_t>(sizeof(nc::asset::MeshVertex));

struct AttributeInfo
{
    uint32_t componentCount = 0;
    uint32_t offset = 0;
    Diligent::VALUE_TYPE type = Diligent::VALUE_TYPE::VT_UNDEFINED;
    nc::graphics::VertexAttribute id = nc::graphics::VertexAttribute::None;
};

constexpr auto g_vertexAttributeInfos = std::array{
    AttributeInfo{3, 0,  Diligent::VT_FLOAT32, nc::graphics::VertexAttribute::Pos},
    AttributeInfo{3, 12, Diligent::VT_FLOAT32, nc::graphics::VertexAttribute::Normal},
    AttributeInfo{2, 24, Diligent::VT_FLOAT32, nc::graphics::VertexAttribute::UV},
    AttributeInfo{4, 32, Diligent::VT_FLOAT32, nc::graphics::VertexAttribute::BoneWeights},
    AttributeInfo{4, 48, Diligent::VT_UINT32,  nc::graphics::VertexAttribute::BoneIds}
};

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

template<class T>
auto MakeBufferData(std::span<const T> data) -> Diligent::BufferData
{
    return Diligent::BufferData{
        data.data(),
        static_cast<uint32_t>(sizeof(T) * data.size())
    };
}

void CreateBuffer(Diligent::IRenderDevice& device,
                  const Diligent::BufferDesc& desc,
                  const Diligent::BufferData& data,
                  Diligent::RefCntAutoPtr<Diligent::IBuffer>& buffer)
{
    buffer.Release();
    device.CreateBuffer(desc, &data, &buffer);
    if (!buffer)
    {
        throw nc::NcError{fmt::format("Failed to create buffer '{}'", desc.Name)};
    }
}

void TransitionBufferStates(Diligent::IDeviceContext& context,
                            Diligent::IBuffer* vertexBuffer,
                            Diligent::IBuffer* indexBuffer)
{
    const auto barriers = std::array{
        Diligent::StateTransitionDesc(
            vertexBuffer,
            Diligent::RESOURCE_STATE_UNKNOWN,
            Diligent::RESOURCE_STATE_VERTEX_BUFFER,
            Diligent::STATE_TRANSITION_FLAG_UPDATE_STATE
        ),
        Diligent::StateTransitionDesc(
            indexBuffer,
            Diligent::RESOURCE_STATE_UNKNOWN,
            Diligent::RESOURCE_STATE_INDEX_BUFFER,
            Diligent::STATE_TRANSITION_FLAG_UPDATE_STATE
        )
    };

    context.TransitionResourceStates(static_cast<uint32_t>(barriers.size()), barriers.data());
}
} // anonymous namespace

namespace nc::graphics
{
auto GetMeshVertexLayoutElements(VertexAttribute attributes,
                                 uint32_t slot,
                                 uint32_t indexOffset) -> std::vector<Diligent::LayoutElement>
{
    auto elements = std::vector<Diligent::LayoutElement>{};

    for (const auto& info : g_vertexAttributeInfos)
    {
        const auto requested = static_cast<bool>(info.id & attributes);
        if (requested)
        {
            elements.emplace_back(
                indexOffset++,
                slot,
                info.componentCount,
                info.type,
                false,
                info.offset,
                g_vertexStride
            );
        }
    }

    return elements;
}

void MeshBuffer::Load(std::span<const asset::MeshVertex> vertices,
                            std::span<const uint32_t> indices,
                            Diligent::IDeviceContext& context,
                            Diligent::IRenderDevice& device)
{
    CreateBuffer(device, MakeVertexBufferDesc(vertices), MakeBufferData(vertices), m_vertexBuffer);
    CreateBuffer(device, MakeIndexBufferDesc(indices), MakeBufferData(indices), m_indexBuffer);
    TransitionBufferStates(context, m_vertexBuffer.RawPtr(), m_indexBuffer.RawPtr());
}

void MeshBuffer::SetBuffers(Diligent::IDeviceContext& context)
{
    NC_ASSERT(m_vertexBuffer && m_indexBuffer, "Mesh buffers are not populated");

    // note: References to buffers are also held by the context. SET_VERTEX_BUFFER_FLAG_RESET is
    //       needed here to release potentially stale buffers.
    context.SetVertexBuffers(
        0,
        1,
        m_vertexBuffer.RawDblPtr(),
        nullptr,
        Diligent::RESOURCE_STATE_TRANSITION_MODE_TRANSITION,
        Diligent::SET_VERTEX_BUFFERS_FLAG_RESET
    );

    context.SetIndexBuffer(
        m_indexBuffer.RawPtr(),
        0,
        Diligent::RESOURCE_STATE_TRANSITION_MODE_TRANSITION
    );
}
} // namespace nc::graphics
