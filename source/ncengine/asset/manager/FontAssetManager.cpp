#include "FontAssetManager.h"

#include "ncengine/asset/AssetData.h"
#include "ncengine/window/Window.h"

#include "imgui.h"

#include <filesystem>

namespace
{
auto GetFontScaling() -> float
{
    const auto [scaleX, scaleY] = nc::window::GetContentScale();
    return std::floor(std::max(scaleX, scaleY));
}

#ifdef NC_TEST_STRIP_DEPENDENCIES
auto LoadFontToAtlas(const char*, float) -> ImFont*
{
    return nullptr;
}

void ClearFontAtlas()
{
}
#else
auto LoadFontToAtlas(const char* path, float size) -> ImFont*
{
    return ImGui::GetIO().Fonts->AddFontFromFileTTF(path, size);
}

void ClearFontAtlas()
{
    return ImGui::GetIO().Fonts->Clear();
}
#endif
} // anonymous namespace

namespace nc::asset
{
FontAssetManager::FontAssetManager(const std::string& assetDirectory)
    : m_fonts{}, m_assetDirectory{assetDirectory}
{
}

auto FontAssetManager::Load(const FontInfo& font, AssetSubtype) -> bool
{
    if (IsLoaded(font))
        return false;

    const auto fullPath = m_assetDirectory + font.path;
    if (!std::filesystem::exists(fullPath))
        throw NcError{fmt::format("Font file does not exist '{}'", fullPath)};

    auto fontHandle = ::LoadFontToAtlas(fullPath.c_str(), font.size * ::GetFontScaling());
    m_fonts.emplace(font, FontView{fontHandle, font.size}); // save original size as its part the 'id'
    m_onUpdate.Emit();
    return true;
}

auto FontAssetManager::Load(std::span<const FontInfo> fonts, AssetSubtype) -> bool
{
    const auto scale = ::GetFontScaling();
    auto anyLoaded = false;
    for(const auto& font : fonts)
    {
        if (IsLoaded(font))
            continue;

        const auto fullPath = m_assetDirectory + font.path;
        if (!std::filesystem::exists(fullPath))
            throw NcError{fmt::format("Font file does not exist '{}'", fullPath)};

        auto fontHandle = ::LoadFontToAtlas(fullPath.c_str(), font.size * scale);
        m_fonts.emplace(font, FontView{fontHandle, font.size});
        anyLoaded = true;
    }

    if (anyLoaded)
        m_onUpdate.Emit();

    return anyLoaded;
}

auto FontAssetManager::Unload(const FontInfo& font) -> bool
{
    if (m_fonts.erase(font) == 0)
        return false;

    ::ClearFontAtlas();
    m_onUpdate.Emit();
    return true;
}

void FontAssetManager::UnloadAll()
{
    ::ClearFontAtlas();
    m_fonts.clear();
    m_onUpdate.Emit();
}

auto FontAssetManager::Acquire(const FontInfo& font) const -> FontView
{
    const auto it = m_fonts.find(font);
    if (it == m_fonts.end())
    {
        throw NcError(fmt::format("Font is not loaded: '{}, {}'", font.path, font.size));
    }

    return it->second;
}

auto FontAssetManager::IsLoaded(const FontInfo& font) const -> bool
{
    return m_fonts.contains(font);
}

auto FontAssetManager::GetAllLoaded() const -> std::vector<std::string_view>
{
    auto out = std::vector<std::string_view>{};
    out.reserve(m_fonts.size());
    std::ranges::transform(m_fonts, std::back_inserter(out), [](const auto& pair)
    {
        return pair.first.path;
    });

    return out;
}
} // namespace nc::asset
