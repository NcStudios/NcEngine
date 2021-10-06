#include "GraphicsResources.h"
#include "graphics/Initializers.h"

#include <iostream>

namespace nc::graphics
{
    MeshesData::MeshesData(ImmutableBuffer<Vertex> vertexBuffer, 
                           ImmutableBuffer<uint32_t> indexBuffer, 
                           std::unordered_map<std::string, Mesh> accessors)
    : m_vertexBuffer{std::move(vertexBuffer)},
      m_indexBuffer{std::move(indexBuffer)},
      m_accessors(std::move(accessors))
    {
    }

    MeshesData::~MeshesData() noexcept
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

    TexturesData::TexturesData(Graphics* graphics, uint32_t maxTexturesCount)
        : m_textures{},
          m_imageInfos{},
          m_accessors{},
          m_descriptorSet{},
          m_descriptorSetLayout{},
          m_sampler{},
          m_layout{},
          m_maxTexturesCount{maxTexturesCount},
          m_texturesInitialized{false}
    {
        // Create and bind the descriptor set for the array of textures.
        std::vector<vk::DescriptorSetLayoutBinding> layoutBindings 
        { 
          CreateDescriptorSetLayoutBinding(0, 1, vk::DescriptorType::eSampler, vk::ShaderStageFlagBits::eFragment),
          CreateDescriptorSetLayoutBinding(1, m_maxTexturesCount, vk::DescriptorType::eSampledImage, vk::ShaderStageFlagBits::eFragment)
        };

        m_descriptorSetLayout = CreateDescriptorSetLayout(graphics, layoutBindings, vk::DescriptorBindingFlagBitsEXT::ePartiallyBound );
        m_descriptorSet = CreateDescriptorSet(graphics, graphics->GetBasePtr()->GetRenderingDescriptorPoolPtr(), 1, &m_descriptorSetLayout.get());
    }

    TexturesData::~TexturesData() noexcept
    {
        for (auto& texture : m_textures)
        {
            texture.image.Clear();
        }

        m_textures.resize(0);
        m_descriptorSet.reset();
        m_descriptorSetLayout.reset();
        m_sampler.reset();
    }

    const std::vector<std::string> TexturesData::GetTexturePaths() const
    {
        std::vector<std::string> texturePaths;
        texturePaths.reserve(m_accessors.size());

        for (auto keyValue : m_accessors)
        {
            texturePaths.push_back(keyValue.first);
        }

        return texturePaths;
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

    void TexturesData::AddTexture(Graphics* graphics, Texture texture)
    {
        auto index = m_accessors.size();
        m_accessors.emplace(texture.uid, index);
        m_textures.push_back(std::move(texture));

        UpdateTextures(graphics);
    }

    void TexturesData::AddTextures(Graphics* graphics, std::vector<Texture> textures)
    {
        for (auto& texture : textures)
        {
            auto index = m_accessors.size();
            m_accessors.emplace(texture.uid, index);
            m_textures.push_back(std::move(texture));
        }

        UpdateTextures(graphics);
    }

    void TexturesData::UpdateTextures(Graphics* graphics)
    {
        m_sampler = graphics->GetBasePtr()->CreateTextureSampler();

        std::array<vk::WriteDescriptorSet, 2> writes;
        vk::DescriptorImageInfo samplerInfo = {};
        samplerInfo.sampler = m_sampler.get();

        if (!m_texturesInitialized)
        {
            m_imageInfos.reserve(m_maxTexturesCount);

            for (uint32_t i = 0; i < m_maxTexturesCount; i++)
            {
                m_imageInfos.push_back(m_textures[0].imageInfo);
            }

            m_texturesInitialized = true;
        }

        writes[0].setDstBinding(0);
        writes[0].setDstArrayElement(0);
        writes[0].setDescriptorType(vk::DescriptorType::eSampler);
        writes[0].setDescriptorCount(1);
        writes[0].setDstSet(m_descriptorSet.get());
        writes[0].setPBufferInfo(0);
        writes[0].setPImageInfo(&samplerInfo);

        std::transform(m_textures.cbegin(), m_textures.cend(), m_imageInfos.begin(), [](const auto& texture)
        {
            return texture.imageInfo;
        });

        writes[1].setDstBinding(1);
        writes[1].setDstArrayElement(0);
        writes[1].setDescriptorType(vk::DescriptorType::eSampledImage);
        writes[1].setDescriptorCount(m_maxTexturesCount);
        writes[1].setDstSet(m_descriptorSet.get());
        writes[1].setPBufferInfo(0);
        writes[1].setPImageInfo(m_imageInfos.data());

        graphics->GetBasePtr()->GetDevice().updateDescriptorSets(2, writes.data(), 0, nullptr);
    }

    void TexturesData::Clear() noexcept
    {
        for (auto& texture : m_textures)
        {
            texture.image.Clear();
        }

        m_textures.resize(0);
    }

    PointLightsData::PointLightsData(Graphics* graphics, uint32_t maxPointLights)
    {
        auto base = graphics->GetBasePtr();

        std::vector<vk::DescriptorSetLayoutBinding> layoutBindings = {CreateDescriptorSetLayoutBinding(0, 1, vk::DescriptorType::eStorageBuffer, vk::ShaderStageFlagBits::eFragment)};
        m_descriptorSetLayout = CreateDescriptorSetLayout(graphics, layoutBindings, vk::DescriptorBindingFlagsEXT());
        m_pointLightsArrayBuffer = WriteableBuffer<nc::PointLightInfo>(graphics, (sizeof(nc::PointLightInfo) * maxPointLights));
        m_descriptorSet = CreateDescriptorSet(graphics, base->GetRenderingDescriptorPoolPtr(), 1, &m_descriptorSetLayout.get());

		vk::DescriptorBufferInfo pointLightArrayInfo;
		pointLightArrayInfo.buffer = *m_pointLightsArrayBuffer.GetBuffer();
		pointLightArrayInfo.offset = 0;
		pointLightArrayInfo.range = sizeof(nc::PointLightInfo) * maxPointLights;

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

    PointLightsData::~PointLightsData() noexcept
    {
        m_pointLightsArrayBuffer.Clear();
        m_descriptorSet.reset();
        m_descriptorSetLayout.reset();
    }

    void PointLightsData::Update(const std::vector<nc::PointLightInfo>& pointLights)
    {
        m_pointLightsArrayBuffer.Map(pointLights, [](nc::PointLightInfo& info)
        {
            info.isInitialized = false;
        });
    }

    vk::DescriptorSetLayout* PointLightsData::GetDescriptorLayout() noexcept
    {
        return &m_descriptorSetLayout.get();
    }

    vk::DescriptorSet* PointLightsData::GetDescriptorSet() noexcept
    {
        return &m_descriptorSet.get();
    }

    ObjectsData::ObjectsData(Graphics* graphics)
    {
        const uint32_t MAX_OBJECTS = 100000;
        const uint32_t objectsSize = (sizeof(ObjectData) * MAX_OBJECTS);
        auto base = graphics->GetBasePtr();

        std::vector<vk::DescriptorSetLayoutBinding> layoutBindings = {CreateDescriptorSetLayoutBinding(0, 1, vk::DescriptorType::eStorageBuffer, vk::ShaderStageFlagBits::eFragment | vk::ShaderStageFlagBits::eVertex)};
        m_descriptorSetLayout = CreateDescriptorSetLayout(graphics, layoutBindings, vk::DescriptorBindingFlagsEXT());
        m_objectsDataBuffer = WriteableBuffer<ObjectData>(graphics, objectsSize); //@todo: not hard code upper bound
        m_descriptorSet = CreateDescriptorSet(graphics, base->GetRenderingDescriptorPoolPtr(), 1, &m_descriptorSetLayout.get());

		vk::DescriptorBufferInfo objectsDataBufferInfo;
		objectsDataBufferInfo.buffer = *m_objectsDataBuffer.GetBuffer();
		objectsDataBufferInfo.offset = 0;
		objectsDataBufferInfo.range = objectsSize;

        vk::WriteDescriptorSet write{};
        write.setDstBinding(0);
        write.setDstArrayElement(0);
        write.setDescriptorType(vk::DescriptorType::eStorageBuffer);
        write.setDescriptorCount(1);
        write.setDstSet(m_descriptorSet.get());
        write.setPBufferInfo(&objectsDataBufferInfo);
        write.setPImageInfo(0);

        base->GetDevice().updateDescriptorSets(1, &write, 0, nullptr);
    }

    ObjectsData::~ObjectsData() noexcept
    {
        m_objectsDataBuffer.Clear();
        m_descriptorSet.reset();
        m_descriptorSetLayout.reset();
    }

    void ObjectsData::Update(const std::vector<ObjectData>& objectsData)
    {
        m_objectsDataBuffer.Map(objectsData);
    }

    vk::DescriptorSetLayout* ObjectsData::GetDescriptorLayout() noexcept
    {
        return &m_descriptorSetLayout.get();
    }

    vk::DescriptorSet* ObjectsData::GetDescriptorSet() noexcept
    {
        return &m_descriptorSet.get();
    }
}