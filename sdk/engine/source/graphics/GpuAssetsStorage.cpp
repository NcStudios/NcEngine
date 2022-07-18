#include "graphics/GpuAssetsStorage.h"
#include "graphics/Base.h"
#include "graphics/GpuAllocator.h"
#include "graphics/Initializers.h"
#include "graphics/resources/TextureManager.h"

namespace nc::graphics
{
GpuAssetsStorage::GpuAssetsStorage(Base* base, GpuAllocator* allocator, const nc::GpuAccessorSignals& gpuAccessorSignals)
    : m_base{base},
      m_allocator{allocator},
      m_vertexBuffer{m_base, m_allocator},
      m_indexBuffer{m_base, m_allocator},
      m_onMeshUpdateConnection{gpuAccessorSignals.onMeshUpdate->Connect(this, &GpuAssetsStorage::UpdateMeshBuffer)},
      m_onTextureUpdateConnection{gpuAccessorSignals.onTextureUpdate->Connect(this, &GpuAssetsStorage::UpdateTextureBuffer)}
{
}

GpuAssetsStorage::~GpuAssetsStorage() noexcept
{
    for(auto& texture : m_textureBuffers)
    {
        texture.image.Clear();
    }

    m_textureBuffers.resize(0);
}

/** Meshes **/
VertexBuffer::VertexBuffer(Base* base, GpuAllocator* allocator)
    : vertices{std::vector<nc::Vertex>{nc::Vertex()}},
      buffer{ImmutableBuffer(base, allocator, vertices)}
{
}

IndexBuffer::IndexBuffer(Base* base, GpuAllocator* allocator)
    : indices{std::vector<uint32_t>{0}},
      buffer{ImmutableBuffer(base, allocator, indices)}
{
}

const VertexBuffer& GpuAssetsStorage::GetVertexData() const noexcept
{
    return m_vertexBuffer;
}

const IndexBuffer& GpuAssetsStorage::GetIndexData() const noexcept
{
    return m_indexBuffer;
}

void GpuAssetsStorage::UpdateMeshBuffer(const MeshBufferData& meshBufferData)	
{
    m_vertexBuffer.buffer.Clear();
    m_vertexBuffer.buffer = ImmutableBuffer(m_base, m_allocator, meshBufferData.vertices);

    m_indexBuffer.buffer.Clear();
    m_indexBuffer.buffer = ImmutableBuffer(m_base, m_allocator, meshBufferData.indices);
}

/** Textures **/
void GpuAssetsStorage::LoadTextureBuffer(const TextureBufferData& textureBufferData)
{
    for (auto i = 0; i < textureBufferData.data.size(); ++i)
    {
        auto& textureData = textureBufferData.data[i];
        auto& textureId = textureBufferData.ids[i];

        m_textureBuffers.emplace_back
        (
            nc::graphics::ImmutableImage(m_base, m_allocator, textureData.pixels, textureData.width, textureData.height),
            textureId
        );

        m_textureImageInfos.emplace_back(nc::graphics::CreateDescriptorImageInfo(&m_sampler.get(), m_textureBuffers[i].image.GetImageView(), vk::ImageLayout::eShaderReadOnlyOptimal));
    }

    graphics::ShaderResourceService<graphics::TextureImageInfo>::Get()->Update(m_textureImageInfos);
}

void GpuAssetsStorage::UnloadTextureBuffer(const TextureBufferData& textureBufferData)
{
    const auto& id = textureBufferData.ids[0];
    auto pos = std::ranges::find_if(m_textureBuffers, [&id](const auto& texture)
    {
        return texture.uid == id;
    });

    assert(pos != m_textureBuffers.end());
    m_textureBuffers.erase(pos);
    graphics::ShaderResourceService<graphics::TextureImageInfo>::Get()->Update(m_textureImageInfos);
}

void GpuAssetsStorage::UnloadAllTextureBuffer()
{
    /** No need to write an empty buffer to the GPU. **/
    m_textureBuffers.clear();
    m_textureImageInfos.clear();
}

void GpuAssetsStorage::UpdateTextureBuffer(const TextureBufferData& textureBufferData)
{
    switch (textureBufferData.updateAction)
    {
        case UpdateAction::Load:
        {
            LoadTextureBuffer(textureBufferData);
            break;
        }
        case UpdateAction::Unload:
        {
            UnloadTextureBuffer(textureBufferData);
            break;
        }
        case UpdateAction::UnloadAll:
        {
            UnloadAllTextureBuffer();
            break;
        }
    }
}
}
