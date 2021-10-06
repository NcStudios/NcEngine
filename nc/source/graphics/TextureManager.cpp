#include "graphics/TextureManager.h"
#include "graphics/Initializers.h"
#include "graphics/resources/ResourceManager.h"
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
    
    void LoadTexture(const std::string& path)
    {
        IF_THROW(!impl, "graphics::LoadMeshAsset - impl is not set");
        impl->LoadTexture(path);
    }
    
    TextureManager::TextureManager(Graphics* graphics)
        : m_graphics{graphics}
    {
        impl = this;
    }

    void TextureManager::LoadTextures(const std::vector<std::string>& paths)
    {
        auto sampler = m_graphics->GetBasePtr()->CreateTextureSampler();
        auto textures = std::vector<Texture>();
        textures.reserve(paths.size());

        // Parse the files into Vulkan image objects
        for (const auto& path : paths)
        {
            if (ResourceManager::TextureExists(path)) continue;

            auto texture = CreateTexture(path, &sampler.get());
            textures.push_back(std::move(texture));
        }

        ResourceManager::AddTextures(m_graphics, std::move(textures));
    }

    void TextureManager::LoadTexture(const std::string& path)
    {
        auto sampler = m_graphics->GetBasePtr()->CreateTextureSampler();

        // Parse the file into a Vulkan image object
        if (ResourceManager::TextureExists(path)) return;

        auto texture = CreateTexture(path, &sampler.get());
        ResourceManager::AddTexture(m_graphics, std::move(texture));
    }

    Texture TextureManager::CreateTexture(const std::string& path, vk::Sampler* sampler)
    {
        int32_t width, height, numChannels;
        stbi_uc* pixels = stbi_load(path.c_str(), &width, &height, &numChannels, STBI_rgb_alpha);

        if (!pixels)
        {
            throw std::runtime_error("Failed to load texture file: " + path);
        }

        Texture texture
        {
            .image = ImmutableImage(m_graphics, pixels, width, height),
            .imageInfo = CreateDescriptorImageInfo(sampler, texture.image.GetImageView(), vk::ImageLayout::eShaderReadOnlyOptimal),
            .uid = path
        };

        return texture;
    }
}