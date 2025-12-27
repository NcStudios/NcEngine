#include "TextureAssetManager.h"
#include "AssetUtilities.h"
#include "asset/Assets.h"
#include "asset/AssetData.h"

#include "ncasset/Import.h"

#include <algorithm>
#include <cstring>
#include <ranges>
#include <string>

namespace
{
template<class T, class Proj>
auto GetSerializableAssetPaths(const std::vector<T>& vec, Proj&& proj) -> std::vector<std::string_view>
{
    auto out = std::vector<std::string_view>{};
    out.reserve(vec.size());

    auto view = vec | std::ranges::views::filter([](const auto& item) { return !item.contains(nc::asset::RuntimeKey); })
                    | std::ranges::views::transform(std::forward<Proj>(proj));

    return std::vector<std::string_view>(view.begin(), view.end());
}
}
namespace nc::asset
{
TextureAssetManager::TextureAssetManager(const std::string& texturesAssetDirectory, uint32_t maxTextures)
    : m_assetDirectory{texturesAssetDirectory},
      m_maxTextureCount{maxTextures},
      m_onUpdate{}
{
}

auto TextureAssetManager::Load(const std::string& path) -> bool
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
    auto texture = asset::TextureWithId{asset::ImportTexture(fullPath), m_table.hash(path)};
    m_table.emplace(path);
    m_onUpdate.Emit(asset::TextureUpdateEventData{
        asset::UpdateAction::Load,
        std::span<const asset::TextureWithId>{&texture, 1}
    });

    return true;
}

auto TextureAssetManager::Load(std::span<const std::string> paths) -> bool
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
        textures.emplace_back(ImportTexture(fullPath), m_table.hash(path));
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

auto TextureAssetManager::LoadFromMemory(const std::string& key, Texture texture) -> bool
{
    auto runtimeTextureKey = RuntimeKey + key;
    if (m_table.size() + 1 >= m_maxTextureCount)
    {
        throw NcError("Cannot exceed max texture count.");
    }

    if (IsLoaded(key))
    {
        return false;
    }

    auto textureWithId = TextureWithId{std::move(texture), m_table.hash(runtimeTextureKey)};
    m_table.emplace(runtimeTextureKey);
    m_onUpdate.Emit(TextureUpdateEventData{
        UpdateAction::Load,
        std::span<const TextureWithId>{&textureWithId, 1}
    });

    return true;
}

auto TextureAssetManager::LoadFromRGBA(const std::string& key,
                                       std::span<const uint8_t> rgbaData,
                                       uint32_t width,
                                       uint32_t height,
                                       TextureFormat format) -> bool
{
    auto texture = Texture{
        .format = format,
        .width = width,
        .height = height,
        .pixelData = std::vector<unsigned char>(rgbaData.begin(), rgbaData.end())
    };
    return LoadFromMemory(key, std::move(texture));
}

auto TextureAssetManager::Unload(const std::string& path) -> bool
{
    auto runtimeTextureKey = RuntimeKey + path;

    if (!m_table.erase(path) && !m_table.erase(runtimeTextureKey))
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
    auto runtimeTextureKey = RuntimeKey + path;
    if (!m_table.contains(path))
    {
        if (!m_table.contains(runtimeTextureKey))
        {
            throw nc::NcError("Texture is not loaded: '{}'", path);
        }
        return Acquire(m_table.hash(runtimeTextureKey));
    }
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

auto TextureAssetManager::GetAllLoaded(bool serializableOnly) const -> std::vector<std::string_view>
{
    if (serializableOnly)
    {
        return GetSerializableAssetPaths(m_table.keys(), [](const auto& data)
        {
            return std::string_view{data};
        });
    }

    return GetPaths(m_table.keys(), [](const auto& data)
    {
        return std::string_view{data};
    });
}

auto TextureAssetManager::GetPath(AssetId id) const -> std::string_view
{
    auto path = m_table.at(m_table.index(id));
    if (path.starts_with(RuntimeKey))
    {
        path.remove_prefix(RuntimeKeyLength);
    }
    return path;
}
} // namespace nc::asset
