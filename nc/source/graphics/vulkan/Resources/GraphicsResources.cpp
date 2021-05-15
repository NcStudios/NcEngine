#include "GraphicsResources.h"

namespace nc::graphics::vulkan
{
    MeshesData::MeshesData(ImmutableBuffer<Vertex> vertexBuffer, 
                           ImmutableBuffer<uint32_t> indexBuffer, 
                           std::unordered_map<std::string, Mesh> accessors)
    : m_vertexBuffer{std::move(vertexBuffer)},
      m_indexBuffer{std::move(indexBuffer)},
      m_accessors(std::move(accessors))
    {
    }

    bool MeshesData::MeshExists(const std::string& uid) const noexcept
    {
        return m_accessors.contains(uid);
    }

    vk::Buffer* MeshesData::GetVertexBuffer()
    {
        return m_vertexBuffer.GetBuffer();
    }

    vk::Buffer* MeshesData::GetIndexBuffer()
    {
        return m_indexBuffer.GetBuffer();
    }

    const Mesh& MeshesData::GetAccessor(const std::string& uid) const
    {
        return m_accessors.at(uid);
    }

    void MeshesData::Clear() noexcept
    {
        m_vertexBuffer.Clear();
        m_indexBuffer.Clear();
        m_accessors.clear();
    }

    TexturesData::TexturesData(std::vector<vulkan::ImmutableImage> textureBuffers, 
                               std::vector<vk::DescriptorImageInfo> imageInfos, 
                               std::unordered_map<std::string, uint32_t> accessors,
                               vk::UniqueDescriptorSet descriptorSet,
                               vk::UniqueDescriptorSetLayout descriptorSetLayout,
                               vk::UniqueSampler sampler,
                               vk::ImageLayout layout)
    : m_textureBuffers{std::move(textureBuffers)},
      m_imageInfos{std::move(imageInfos)},
      m_accessors{std::move(accessors)},
      m_descriptorSet{std::move(descriptorSet)},
      m_descriptorSetLayout{std::move(descriptorSetLayout)},
      m_sampler{std::move(sampler)},
      m_layout{layout}
    {
    }

    bool TexturesData::TextureExists(const std::string& uid) const noexcept
    {
        return m_accessors.contains(uid);
    }

    vk::DescriptorSetLayout* TexturesData::GetDescriptorLayout() noexcept
    {
        return &m_descriptorSetLayout.get();
    }

    vk::DescriptorSet* TexturesData::GetDescriptorSet() noexcept
    {
        return &m_descriptorSet.get();
    }

    uint32_t TexturesData::GetAccessor(const std::string& uid) const
    {
        return m_accessors.at(uid);
    }

    void TexturesData::Clear() noexcept
    {
        for (auto& texture : m_textureBuffers)
        {
            texture.Clear();
        }

        m_textureBuffers.resize(0);
        m_descriptorSet.reset();
        m_descriptorSetLayout.reset();
        m_sampler.reset();
    }
}