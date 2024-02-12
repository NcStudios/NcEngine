#include "FontAssetManager.h"

#include "ncengine/asset/AssetData.h"
#include "ncengine/window/Window.h"

#include "imgui/imgui.h"

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

namespace nc
{
FontAssetManager::FontAssetManager(const std::string& assetDirectory)
    : m_fonts{}, m_assetDirectory{assetDirectory}
{
}

bool FontAssetManager::Load(const FontInfo& font, bool isExternal, asset_flags_type)
{
    if (IsLoaded(font))
        return false;

    const auto fullPath = isExternal ? font.path : m_assetDirectory + font.path;
    if (!std::filesystem::exists(fullPath))
        throw NcError{fmt::format("Font file does not exist '{}'", fullPath)};

    const auto size = font.size * ::GetFontScaling();
    auto fontHandle = ::LoadFontToAtlas(fullPath.c_str(), size);
    m_fonts.emplace(font, FontView{fontHandle, size});
    m_onUpdate.Emit();
    return true;
}

bool FontAssetManager::Load(std::span<const FontInfo> fonts, bool isExternal, asset_flags_type)
{
    const auto scale = ::GetFontScaling();
    auto anyLoaded = false;
    for(const auto& font : fonts)
    {
        if (IsLoaded(font))
            continue;

        const auto fullPath = isExternal ? font.path : m_assetDirectory + font.path;
        if (!std::filesystem::exists(fullPath))
            throw NcError{fmt::format("Font file does not exist '{}'", fullPath)};

        const auto size = font.size * scale;
        auto fontHandle = ::LoadFontToAtlas(fullPath.c_str(), size);
        m_fonts.emplace(font, FontView{fontHandle, size});
        anyLoaded = true;
    }

    if (anyLoaded)
        m_onUpdate.Emit();

    return anyLoaded;
}

bool FontAssetManager::Unload(const FontInfo& font, asset_flags_type)
{
    if (m_fonts.erase(font) == 0)
        return false;

    ::ClearFontAtlas();
    m_onUpdate.Emit();
    return true;
}

void FontAssetManager::UnloadAll(asset_flags_type)
{
    ::ClearFontAtlas();
    m_fonts.clear();
    m_onUpdate.Emit();
}

auto FontAssetManager::Acquire(const FontInfo& font, asset_flags_type) const -> FontView
{
    const auto it = m_fonts.find(font);
    if (it == m_fonts.end())
    {
        throw NcError(fmt::format("Font is not loaded: '{}, {}'", font.path, font.size));
    }

    return it->second;
}

bool FontAssetManager::IsLoaded(const FontInfo& font, asset_flags_type) const
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

auto FontAssetManager::OnUpdate() -> Signal<>&
{
    return m_onUpdate;
}
} // namespace nc
