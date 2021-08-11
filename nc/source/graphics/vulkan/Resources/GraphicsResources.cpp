#include "GraphicsResources.h"
#include "graphics/vulkan/Initializers.h"

#include <iostream>

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

    MeshesData::~MeshesData()
    {
        Clear();
    }

    const std::vector<std::string> MeshesData::GetMeshPaths() const
    {
        std::vector<std::string> meshPaths;
        meshPaths.reserve(m_accessors.size());

        for (auto keyValue : m_accessors)
        {
            meshPaths.push_back(keyValue.first);
        }

        return meshPaths;
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

    TexturesData::~TexturesData()
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
    }

    PointLightsData::PointLightsData(Graphics2* graphics, uint32_t maxPointLights)
    {
        auto base = graphics->GetBasePtr();

        std::vector<vk::DescriptorSetLayoutBinding> layoutBindings = {CreateDescriptorSetLayoutBinding(0, 1, vk::DescriptorType::eStorageBuffer, vk::ShaderStageFlagBits::eFragment)};
        m_descriptorSetLayout = CreateDescriptorSetLayout(graphics, layoutBindings, vk::DescriptorBindingFlagsEXT());
        m_pointLightsArrayBuffer = WriteableBuffer<nc::vulkan::PointLightInfo>(graphics, (sizeof(nc::vulkan::PointLightInfo) * maxPointLights));
        m_descriptorSet = CreateDescriptorSet(graphics, base->GetRenderingDescriptorPoolPtr(), 1, &m_descriptorSetLayout.get());

		vk::DescriptorBufferInfo pointLightArrayInfo;
		pointLightArrayInfo.buffer = *m_pointLightsArrayBuffer.GetBuffer();
		pointLightArrayInfo.offset = 0;
		pointLightArrayInfo.range = sizeof(nc::vulkan::PointLightInfo) * maxPointLights;

        vk::WriteDescriptorSet write{};
        write.setDstBinding(0);
        write.setDstArrayElement(0);
        write.setDescriptorType(vk::DescriptorType::eStorageBuffer);
        write.setDescriptorCount(1);
        write.setDstSet(m_descriptorSet.get());
        write.setPBufferInfo(&pointLightArrayInfo);
        write.setPImageInfo(0);

        base->GetDevice().updateDescriptorSets(1, &write, 0, nullptr);
    }

    PointLightsData::~PointLightsData()
    {
        m_pointLightsArrayBuffer.Clear();
        m_descriptorSet.reset();
        m_descriptorSetLayout.reset();
    }

    void PointLightsData::Update(const std::vector<nc::vulkan::PointLightInfo>& pointLights)
    {
        m_pointLightsArrayBuffer.Map(pointLights);
    }

    vk::DescriptorSetLayout* PointLightsData::GetDescriptorLayout() noexcept
    {
        return &m_descriptorSetLayout.get();
    }

    vk::DescriptorSet* PointLightsData::GetDescriptorSet() noexcept
    {
        return &m_descriptorSet.get();
    }
}