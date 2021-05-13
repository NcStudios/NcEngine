#include "graphics/vulkan/TextureManager.h"
#include "graphics/vulkan/Initializers.h"
#include "graphics/vulkan/resources/ResourceManager.h"
#include "graphics/vulkan/resources/GraphicsResources.h"
#include "debug/Utils.h"
#include "config/Config.h"
#include "config/ConfigInternal.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

namespace
{
    using namespace nc::graphics::vulkan;
    TextureManager* impl = nullptr;
} // end anonymous namespace

namespace nc::graphics::vulkan
{
    void LoadTextures(const std::vector<std::string>& paths)
    {
        IF_THROW(!impl, "graphics::vulkan::LoadMeshAsset - impl is not set");
        impl->LoadTextures(paths);
    }
    
    TextureManager::TextureManager(nc::graphics::Graphics2* graphics)
    : m_graphics{graphics}
    {
        impl = this;
    }

    void TextureManager::LoadTextures(const std::vector<std::string>& paths)
    {
        TexturesData textures{};

        textures.sampler = m_graphics->GetBasePtr()->CreateTextureSampler();
        textures.layout = vk::ImageLayout::eShaderReadOnlyOptimal;

        uint32_t index = 0;
        for (auto& path : paths)
        {
            if (ResourceManager::TextureExists(path)) continue;
            
            int32_t width, height, numChannels;

            stbi_uc* pixels = stbi_load(path.c_str(), &width, &height, &numChannels, STBI_rgb_alpha);

            if (!pixels)
            {
                throw std::runtime_error("Failed to load texture file: " + path);
            }

            auto image = std::make_unique<ImmutableImage>();
            textures.textureBuffers.push_back(std::move(image));
            textures.textureBuffers.back()->Bind(m_graphics, pixels, width, height);
            textures.accessors.emplace(std::make_pair(path, index));
            textures.imageInfos.push_back(CreateDescriptorImageInfo(&textures.sampler.get(), textures.textureBuffers[index]->GetImageView(), vk::ImageLayout::eShaderReadOnlyOptimal));
            index++;
        }

        CreateDescriptorSet(&textures);
        WriteDescriptorImages(&textures, textures.accessors.size());

        ResourceManager::AddTextures(std::move(textures));
    }

    void TextureManager::CreateDescriptorSet(TexturesData* textures)
    {
        vk::DescriptorSetLayoutBinding layoutBindings[2];
        layoutBindings[0].setBinding(0);
        layoutBindings[0].setDescriptorCount(1);
        layoutBindings[0].setDescriptorType(vk::DescriptorType::eSampler);
        layoutBindings[0].setPImmutableSamplers(nullptr);
        layoutBindings[0].setStageFlags(vk::ShaderStageFlagBits::eFragment);

        layoutBindings[1].setBinding(1);
        layoutBindings[1].setDescriptorCount(textures->accessors.size());
        layoutBindings[1].setDescriptorType(vk::DescriptorType::eSampledImage);
        layoutBindings[1].setPImmutableSamplers(nullptr);
        layoutBindings[1].setStageFlags(vk::ShaderStageFlagBits::eFragment);

        vk::DescriptorBindingFlagsEXT bindFlag = vk::DescriptorBindingFlagBitsEXT::ePartiallyBound;

        vk::DescriptorSetLayoutBindingFlagsCreateInfoEXT extendedInfo{};
        extendedInfo.setPNext(nullptr);
        extendedInfo.setBindingCount(2);
        extendedInfo.setPBindingFlags(&bindFlag);

        vk::DescriptorSetLayoutCreateInfo setInfo{};
        setInfo.setBindingCount(2);
        setInfo.setFlags(vk::DescriptorSetLayoutCreateFlags());
        setInfo.setPNext(&extendedInfo);
        setInfo.setPBindings(layoutBindings);

        if (m_graphics->GetBasePtr()->GetDevice().createDescriptorSetLayout(&setInfo, nullptr, &(textures->descriptorSetLayout)) != vk::Result::eSuccess)
        {
            throw std::runtime_error("Failed to create descriptor set layout.");
        }

        vk::DescriptorSetAllocateInfo allocationInfo{};
        allocationInfo.setPNext(nullptr);
        allocationInfo.setDescriptorPool(*(m_graphics->GetBasePtr()->GetRenderingDescriptorPoolPtr()));
        allocationInfo.setDescriptorSetCount(1);
        allocationInfo.setPSetLayouts(&(textures->descriptorSetLayout));

        if (m_graphics->GetBasePtr()->GetDevice().allocateDescriptorSets(&allocationInfo, &(textures->descriptorSet)) != vk::Result::eSuccess)
        {
            throw std::runtime_error("Failed to allocate descriptor sets.");
        }
    }

    void TextureManager::WriteDescriptorImages(TexturesData* textures, uint32_t arraySize)
    {
        std::array<vk::WriteDescriptorSet, 2> writes;
        vk::DescriptorImageInfo samplerInfo = {};
        samplerInfo.sampler = textures->sampler.get();

        textures->imageInfos.reserve(arraySize);

        writes[0].setDstBinding(0);
        writes[0].setDstArrayElement(0);
        writes[0].setDescriptorType(vk::DescriptorType::eSampler);
        writes[0].setDescriptorCount(1);
        writes[0].setDstSet(textures->descriptorSet);
        writes[0].setPBufferInfo(0);
        writes[0].setPImageInfo(&samplerInfo);

        writes[1].setDstBinding(1);
        writes[1].setDstArrayElement(0);
        writes[1].setDescriptorType(vk::DescriptorType::eSampledImage);
        writes[1].setDescriptorCount(arraySize);
        writes[1].setDstSet(textures->descriptorSet);
        writes[1].setPBufferInfo(0);
        writes[1].setPImageInfo(textures->imageInfos.data());

        m_graphics->GetBasePtr()->GetDevice().updateDescriptorSets(2, writes.data(), 0, nullptr);
    }

    vk::DescriptorImageInfo TextureManager::CreateDescriptorImageInfo(vk::Sampler* sampler, const vk::ImageView& imageView, vk::ImageLayout layout)
    {
        vk::DescriptorImageInfo imageInfo = {};
        imageInfo.setSampler(*sampler);
        imageInfo.setImageView(imageView);
        imageInfo.setImageLayout(layout);
        return imageInfo;
    }
}