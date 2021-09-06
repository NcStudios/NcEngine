#include "graphics/TextureManager.h"
#include "graphics/Initializers.h"
#include "graphics/resources/ResourceManager.h"
#include "graphics/resources/GraphicsResources.h"
#include "debug/Utils.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

namespace
{
    using namespace nc::graphics;
    TextureManager* impl = nullptr;
} // end anonymous namespace

namespace nc::graphics
{
    void LoadTextures(const std::vector<std::string>& paths)
    {
        IF_THROW(!impl, "graphics::LoadMeshAsset - impl is not set");
        impl->LoadTextures(paths);
    }
    
    TextureManager::TextureManager(nc::graphics::Graphics2* graphics)
    : m_graphics{graphics}
    {
        impl = this;
    }

    void TextureManager::LoadTextures(const std::vector<std::string>& paths)
    {
        auto imagesCount = paths.size();

        std::vector<vk::DescriptorImageInfo> imageInfos = {};
        std::vector<ImmutableImage> images = {};
        std::unordered_map<std::string, uint32_t> accessors = {};
        std::vector<std::string> pathsToLoad;

        auto texturesExist = ResourceManager::HasTextures();
        if (texturesExist)
        {
            pathsToLoad = ResourceManager::GetTexturePaths();
            pathsToLoad.insert(paths.end(), paths.begin(), paths.end());
        }
        else
        {
            pathsToLoad = paths;
        }

        auto sampler = m_graphics->GetBasePtr()->CreateTextureSampler();
        auto layoutType = vk::ImageLayout::eShaderReadOnlyOptimal;

        int32_t width, height, numChannels;
        uint32_t index = 0;

        // Parse the files into Vulkan image objects and bind them to the GPU as an array of textures.
        for (auto& path : pathsToLoad)
        {
            if (ResourceManager::TextureExists(path)) continue;

            stbi_uc* pixels = stbi_load(path.c_str(), &width, &height, &numChannels, STBI_rgb_alpha);

            if (!pixels)
            {
                throw std::runtime_error("Failed to load texture file: " + path);
            }

            images.emplace_back(m_graphics, pixels, width, height);
            auto imageInfo = CreateDescriptorImageInfo(&sampler.get(), images[index].GetImageView(), vk::ImageLayout::eShaderReadOnlyOptimal);
            imageInfos.push_back(imageInfo);
            accessors.emplace(path, index);
            index++;
        }

        // Create and bind the descriptor set for the array of textures.
        std::vector<vk::DescriptorSetLayoutBinding> layoutBindings 
        { 
          CreateDescriptorSetLayoutBinding(0, 1, vk::DescriptorType::eSampler, vk::ShaderStageFlagBits::eFragment),
          CreateDescriptorSetLayoutBinding(1, imagesCount, vk::DescriptorType::eSampledImage, vk::ShaderStageFlagBits::eFragment)
        };

        auto layout = CreateDescriptorSetLayout(m_graphics, layoutBindings, vk::DescriptorBindingFlagBitsEXT::ePartiallyBound );
        auto descriptorSet = CreateDescriptorSet(m_graphics, m_graphics->GetBasePtr()->GetRenderingDescriptorPoolPtr(), 1, &layout.get());

        std::array<vk::WriteDescriptorSet, 2> writes;
        vk::DescriptorImageInfo samplerInfo = {};
        samplerInfo.sampler = sampler.get();

        imageInfos.reserve(imagesCount);

        writes[0].setDstBinding(0);
        writes[0].setDstArrayElement(0);
        writes[0].setDescriptorType(vk::DescriptorType::eSampler);
        writes[0].setDescriptorCount(1);
        writes[0].setDstSet(descriptorSet.get());
        writes[0].setPBufferInfo(0);
        writes[0].setPImageInfo(&samplerInfo);

        writes[1].setDstBinding(1);
        writes[1].setDstArrayElement(0);
        writes[1].setDescriptorType(vk::DescriptorType::eSampledImage);
        writes[1].setDescriptorCount(imagesCount);
        writes[1].setDstSet(descriptorSet.get());
        writes[1].setPBufferInfo(0);
        writes[1].setPImageInfo(imageInfos.data());

        m_graphics->GetBasePtr()->GetDevice().updateDescriptorSets(2, writes.data(), 0, nullptr);

        // @todo: replace above with below once I figure out the bug.
        // std::array<vk::WriteDescriptorSet, 2> writeDescriptorSets = {};
        // { 
        //     CreateSamplerDescriptorWrite(&sampler.get(), &descriptorSet.get(), 0),
        //     CreateImagesDescriptorWrite(&descriptorSet.get(), &imageInfos, imagesCount, 1)
        // };
        // m_graphics->GetBasePtr()->GetDevice().updateDescriptorSets(2, writeDescriptorSets.data(), 0, nullptr);

        auto texturesData = std::make_unique<TexturesData>(std::move(images), std::move(imageInfos), std::move(accessors), std::move(descriptorSet), std::move(layout), std::move(sampler), layoutType);

        // Send the vulkan objects over to Resource Manager.
        ResourceManager::LoadTextures(std::move(texturesData));
    }
}