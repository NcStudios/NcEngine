#include "TextureAssetManager.h"
#include "AssetUtilities.h"
#include "asset/Assets.h"
#include "asset/AssetData.h"

#include "ncasset/Import.h"

#include <ranges>

namespace nc::asset
{
TextureAssetManager::TextureAssetManager(const std::string& texturesAssetDirectory, uint32_t maxTextures)
    : m_assetDirectory{texturesAssetDirectory},
      m_maxTextureCount{maxTextures},
      m_onUpdate{}
{
}

bool TextureAssetManager::Load(const std::string& path, AssetSubtype subtype)
{
    if (m_table.size() + 1 >= m_maxTextureCount)
    {
        throw NcError("Cannot exceed max texture count.");
    }

    if (IsLoaded(path))
    {
        return false;
    }

    const auto fullPath = m_assetDirectory + path;
    auto texture = asset::TextureWithId{asset::ImportTexture(fullPath), m_table.hash(path), subtype};
    m_table.emplace(path);
    m_onUpdate.Emit(asset::TextureUpdateEventData{
        asset::UpdateAction::Load,
        std::span<const asset::TextureWithId>{&texture, 1}
    });

    return true;
}

bool TextureAssetManager::Load(std::span<const std::string> paths, std::span<const AssetSubtype> subtypes)
{
    NC_ASSERT(paths.size() == subtypes.size(), "Number of subtypes does not match number of asset paths");
    if (m_table.size() + paths.size() >= m_maxTextureCount)
    {
        throw NcError("Cannot exceed max texture count.");
    }

    auto textures = std::vector<TextureWithId>{};
    textures.reserve(paths.size());

    for (const auto [path, subtype] : std::views::zip(paths, subtypes))
    {
        if (IsLoaded(path))
        {
            continue;
        }

        const auto fullPath = m_assetDirectory + path;
        m_table.emplace(path);
        textures.emplace_back(ImportTexture(fullPath), m_table.hash(path), subtype);
    }

    if (!textures.empty())
    {
        m_onUpdate.Emit(TextureUpdateEventData{
            UpdateAction::Load,
            std::span<const TextureWithId>{textures}
        });
    }

    return true;
}

bool TextureAssetManager::Load(std::span<const std::string> paths, AssetSubtype subtype)
{
    if (m_table.size() + paths.size() >= m_maxTextureCount)
    {
        throw NcError("Cannot exceed max texture count.");
    }

    auto textures = std::vector<TextureWithId>{};
    textures.reserve(paths.size());

    for (const auto& path : paths)
    {
        if (IsLoaded(path))
        {
            continue;
        }

        m_table.emplace(path);
        const auto fullPath = m_assetDirectory + path;
        textures.emplace_back(ImportTexture(fullPath), m_table.hash(path), subtype);
    }

    if (!textures.empty())
    {
        m_onUpdate.Emit(TextureUpdateEventData{
            UpdateAction::Load,
            std::span<const TextureWithId>{textures}
        });
    }

    return true;
}

bool TextureAssetManager::Unload(const std::string& path)
{
    if (!m_table.erase(path))
        return false;

    m_onUpdate.Emit(TextureUpdateEventData{
        UpdateAction::Unload,
        std::span<const TextureWithId>{}
    });

    return true;
}

void TextureAssetManager::UnloadAll()
{
    m_table.clear();
    m_onUpdate.Emit(TextureUpdateEventData{
        UpdateAction::UnloadAll,
        {}
    });
}

auto TextureAssetManager::Acquire(const std::string& path) const -> TextureView
{
    NC_ASSERT(m_table.contains(path), fmt::format("Texture is not loaded: '{}'", path));
    return Acquire(m_table.hash(path));
}

auto TextureAssetManager::Acquire(AssetId id) const -> TextureView
{
    const auto index = m_table.index(id);
    NC_ASSERT(index != m_table.NullIndex, fmt::format("Texture is not loaded: '{}'", id));
    return TextureView{
        .id = id,
        .index = static_cast<uint32_t>(index)
    };
}

bool TextureAssetManager::IsLoaded(const std::string& path) const
{
    return m_table.contains(path);
}

auto TextureAssetManager::OnUpdate() -> Signal<const TextureUpdateEventData&>&
{
    return m_onUpdate;
}

auto TextureAssetManager::GetAllLoaded() const -> std::vector<std::string_view>
{
    return GetPaths(m_table.keys(), [](const auto& data)
    {
        return std::string_view{data};
    });
}
} // namespace nc::asset
