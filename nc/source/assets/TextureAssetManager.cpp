#include "TextureAssetManager.h"
#include "graphics/Initializers.h"
#include "graphics/resources/TextureManager.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"
#undef STB_IMAGE_IMPLEMENATION

#include <cassert>
#include <fstream>

namespace
{
    auto ReadTexture(const std::string& path, nc::graphics::Graphics* graphics, vk::Sampler* sampler) -> nc::graphics::Texture
    {
        int32_t width, height, numChannels;
        stbi_uc* pixels = stbi_load(path.c_str(), &width, &height, &numChannels, STBI_rgb_alpha);

        if(!pixels)
            throw nc::NcError("Failed to load texture file: " + path);

        nc::graphics::Texture texture
        {
            .image = nc::graphics::ImmutableImage(graphics, pixels, width, height),
            .imageInfo = nc::graphics::CreateDescriptorImageInfo(sampler, texture.image.GetImageView(), vk::ImageLayout::eShaderReadOnlyOptimal),
            .uid = path
        };

        return texture;
    }
}

namespace nc
{
    TextureAssetManager::TextureAssetManager(graphics::Graphics* graphics, const std::string& texturesAssetDirectory, uint32_t maxTextures)
        : m_accessors{},
          m_textures{},
          m_graphics{graphics},
          m_assetDirectory{texturesAssetDirectory},
          m_sampler{m_graphics->GetBasePtr()->CreateTextureSampler()},
          m_maxTextureCount{maxTextures}
    {
    }

    TextureAssetManager::~TextureAssetManager() noexcept
    {
        for(auto& texture : m_textures)
        {
            texture.image.Clear();
        }

        m_textures.resize(0);
    }

    bool TextureAssetManager::Load(const std::string& path, bool isExternal)
    {
        const auto index = static_cast<uint32_t>(m_accessors.size());

        if (index + 1 >= m_maxTextureCount)
            throw NcError("Cannot exceed max texture count.");

        if(IsLoaded(path))
            return false;

        m_accessors.emplace(path, index);
        const auto fullPath = isExternal ? path : m_assetDirectory + path;
        m_textures.push_back(ReadTexture(fullPath, m_graphics, &m_sampler.get()));
        graphics::ShaderResourceService<graphics::Texture>::Get()->Update(m_textures);
        return true;
    }

    bool TextureAssetManager::Load(std::span<const std::string> paths, bool isExternal)
    {
        const auto newTextureCount = static_cast<uint32_t>(paths.size());
        auto nextTextureIndex = static_cast<uint32_t>(m_textures.size());
        if (newTextureCount + nextTextureIndex >= m_maxTextureCount)
            throw NcError("Cannot exceed max texture count.");

        for (const auto& path : paths)
        {
            if(IsLoaded(path))
                continue;

            const auto fullPath = isExternal ? path : m_assetDirectory + path;
            m_textures.push_back(ReadTexture(fullPath, m_graphics, &m_sampler.get()));
            m_accessors.emplace(path, nextTextureIndex++);
        }

        graphics::ShaderResourceService<graphics::Texture>::Get()->Update(m_textures);
        return true;
    }

    bool TextureAssetManager::Unload(const std::string& path)
    {
        auto removed = static_cast<bool>(m_accessors.erase(path));
        if(!removed)
            return false;
        
        auto pos = std::ranges::find_if(m_textures, [&path](const auto& texture)
        {
            return texture.uid == path;
        });

        assert(pos != m_textures.end());
        auto index = std::distance(m_textures.begin(), pos);
        m_textures.erase(pos);

        for(auto& [unused, textureView] : m_accessors)
        {
            if(textureView.index > index)
                --textureView.index;
        }

        graphics::ShaderResourceService<graphics::Texture>::Get()->Update(m_textures);
        return true;
    }

    void TextureAssetManager::UnloadAll()
    {
        m_accessors.clear();
        m_textures.clear();
    }

    auto TextureAssetManager::Acquire(const std::string& path) const -> TextureView
    {
        const auto it = m_accessors.find(path);
        if(it == m_accessors.cend())
            throw NcError("Asset is not loaded: " + path);
        
        return it->second;
    }
    
    bool TextureAssetManager::IsLoaded(const std::string& path) const
    {
        return m_accessors.contains(path);
    }
}