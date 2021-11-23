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
    TextureAssetManager::TextureAssetManager(graphics::Graphics* graphics, uint32_t maxTextures)
        : m_textureAccessors{},
          m_textures{},
          m_graphics{graphics},
          m_textureSampler{m_graphics->GetBasePtr()->CreateTextureSampler()},
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

    bool TextureAssetManager::Load(const std::string& path)
    {
        const auto index = m_textureAccessors.size();

        if (index + 1 >= m_maxTextureCount)
            throw NcError("Cannot exceed max texture count.");

        if(IsLoaded(path))
            return false;

        m_textureAccessors.emplace(path, index);
        m_textures.push_back(ReadTexture(path, m_graphics, &m_textureSampler.get()));
        graphics::ShaderResourceService<graphics::Texture>::Get()->Update(m_textures);
        return true;
    }

    bool TextureAssetManager::Load(std::span<const std::string> paths)
    {
        const auto newTextureCount = paths.size();
        auto nextTextureIndex = m_textures.size();
        if (newTextureCount + nextTextureIndex >= m_maxTextureCount)
            throw NcError("Cannot exceed max texture count.");

        for (const auto& path : paths)
        {
            if(IsLoaded(path))
                continue;

            m_textures.push_back(ReadTexture(path, m_graphics, &m_textureSampler.get()));
            m_textureAccessors.emplace(path, nextTextureIndex++);
        }

        graphics::ShaderResourceService<graphics::Texture>::Get()->Update(m_textures);
        return true;
    }

    bool TextureAssetManager::Unload(const std::string& path)
    {
        auto removed = static_cast<bool>(m_textureAccessors.erase(path));
        if(!removed)
            return false;
        
        auto pos = std::ranges::find_if(m_textures, [&path](const auto& texture)
        {
            return texture.uid == path;
        });

        assert(pos != m_textures.end());
        auto index = std::distance(m_textures.begin(), pos);
        m_textures.erase(pos);

        for(auto& [path, textureView] : m_textureAccessors)
        {
            if(textureView.index > index)
                --textureView.index;
        }

        graphics::ShaderResourceService<graphics::Texture>::Get()->Update(m_textures);
        return true;
    }

    void TextureAssetManager::UnloadAll()
    {
        m_textureAccessors.clear();
        m_textures.clear();
    }

    auto TextureAssetManager::Acquire(const std::string& path) const -> TextureView
    {
        const auto it = m_textureAccessors.find(path);
        if(it == m_textureAccessors.cend())
            throw NcError("Asset is not loaded: " + path);
        
        return it->second;
    }
    
    bool TextureAssetManager::IsLoaded(const std::string& path) const
    {
        return m_textureAccessors.contains(path);
    }
}