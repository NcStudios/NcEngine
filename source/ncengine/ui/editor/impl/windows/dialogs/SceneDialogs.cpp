#include "SceneDialogs.h"
#include "ui/editor/impl/SandboxScene.h"
#include "ncengine/serialize/SceneSerialization.h"

#include "fmt/chrono.h"

#include <chrono>
#include <filesystem>
#include <fstream>

namespace
{
auto BuildSimpleFilter(bool includePersistent)
{
    return std::move_only_function<bool(nc::Entity)>{[includePersistent](nc::Entity entity)
    {
        return includePersistent ? true : !entity.IsPersistent();
    }};
}

auto BuildLayerFilter(bool includePersistent, const std::vector<uint8_t>& includedLayers)
{
    auto layerFlags = std::vector<bool>(256ull, false);
    for (auto& included : includedLayers)
    {
        layerFlags.at(included) = true;
    }

    return std::move_only_function<bool(nc::Entity)>{[flags = std::move(layerFlags), includePersistent](nc::Entity entity)
    {
        return (!includePersistent && entity.IsPersistent()) ? false : flags.at(entity.Layer());
    }};
}

[[nodiscard]] auto BackupIfExists(const std::string& filename) -> std::error_code
{
    auto error = std::error_code{};
    auto path = std::filesystem::path{filename};
    if (std::filesystem::exists(path))
    {
        const auto backupName = fmt::format(
            "{}_backup_{:%Y-%m-%d_%H-%M-%S}",
            path.stem().string(),
            std::chrono::system_clock::now()
        );

        path.replace_filename(backupName);
        std::filesystem::copy(filename, path, error);
    }

    return error;
}
} // anonymous namespace

namespace nc::ui::editor
{
void NewSceneDialog::Draw(const EditorContext& ctx)
{
    DrawPopup("Open Sandbox Scene", ctx.dimensions, [&]()
    {
        ImGui::TextWrapped("%s", "Are you sure you want to open a new sandbox scene?");
        if (ImGui::Button("Create Scene"))
        {
            m_ncScene->Queue(std::make_unique<SandboxScene>(ctx.objectBucket, ctx.editorCamera));
            m_ncScene->ScheduleTransition();
            ClosePopup();
        }
    });
}

void SaveSceneDialog::Open(asset::AssetMap assets)
{
    m_errorText.clear();
    m_assets = std::move(assets);
    OpenPopup();
}

void SaveSceneDialog::Draw(const ImVec2& dimensions)
{
    DrawPopup("Save Scene Fragment", dimensions, [&]()
    {
        ImGui::Text("%s", "Include:");
        Checkbox(m_includeAssets, "Loaded Assets");
        Checkbox(m_includePersistent, "Persistent Entities");
        ImGui::RadioButton("All Layers", &m_layerInclusionType, LayerInclusionAll);
        ImGui::RadioButton("Filter Layers", &m_layerInclusionType, LayerInclusionFilter);
        if (m_layerInclusionType == LayerInclusionFilter)
        {
            DrawLayerFilterListBox();
        }

        SeparatorSpaced();
        Checkbox(m_backup, "Backup on overwrite");
        InputText(m_fileName, "filename");
        if (ImGui::Button("save"))
        {
            OnSave();
        }

        ImGui::TextWrapped("%s", m_errorText.c_str());
    });
}

void SaveSceneDialog::DrawLayerFilterListBox()
{
    static constexpr auto selectableSize = ImVec2{0.0f, 19.0f};
    if (ImGui::BeginListBox("##layerfilters"))
    {
        for (auto [i, value] : std::views::enumerate(m_includedLayers))
        {
            IMGUI_SCOPE(ImGuiId, static_cast<int>(i));
            const auto selected = i == m_selectedLayer;
            if (SelectableWidget(selected, selectableSize, [&](){ InputU8(value, "##value"); }))
                m_selectedLayer = i;
        }

        ImGui::EndListBox();
    }

    if (ImGui::Button("+"))
        AddLayer();

    ImGui::SameLine();

    ImGui::BeginDisabled(static_cast<size_t>(m_selectedLayer) >= m_includedLayers.size());
    if (ImGui::Button("-"))
        RemoveSelectedLayer();
    ImGui::EndDisabled();

    ImGui::SameLine();

    if (ImGui::Button("Add All"))
        EnableAllLayers();

    ImGui::SameLine();

    if (ImGui::Button("Remove All"))
        DisableAllLayers();
}

void SaveSceneDialog::AddLayer()
{
    m_includedLayers.push_back(0);
}

void SaveSceneDialog::RemoveSelectedLayer()
{
    m_includedLayers.erase(m_includedLayers.begin() + m_selectedLayer);
}

void SaveSceneDialog::EnableAllLayers()
{
    auto allLayers = std::views::iota(uint32_t{0}, uint32_t{256});
    m_includedLayers = std::vector<uint8_t>{allLayers.cbegin(), allLayers.cend()};
    m_selectedLayer = static_cast<ptrdiff_t>(m_includedLayers.size());
}

void SaveSceneDialog::DisableAllLayers()
{
    m_includedLayers.clear();
    m_selectedLayer = static_cast<ptrdiff_t>(m_includedLayers.size());
}

void SaveSceneDialog::OnSave()
{
    if (m_backup)
    {
        if (auto error = ::BackupIfExists(m_fileName))
        {
            m_errorText = fmt::format("Error backing up file: '{}'.", error.message());
            return;
        }
    }

    if (auto file = std::ofstream{m_fileName, std::ios::binary | std::ios::trunc})
    {
        const auto& assets = m_includeAssets ? m_assets : asset::AssetMap{};
        SaveSceneFragment(file, m_world, assets, SelectFilter());
        ClosePopup();
    }
    else
    {
        m_errorText = fmt::format("Error saving file '{}'.", m_fileName);
    }
}

auto SaveSceneDialog::SelectFilter() -> std::move_only_function<bool(Entity)>
{
    return m_layerInclusionType == LayerInclusionAll
        ? BuildSimpleFilter(m_includePersistent)
        : BuildLayerFilter(m_includePersistent, m_includedLayers);
}

void LoadSceneDialog::Open()
{
    m_errorText.clear();
    OpenPopup();
}

void LoadSceneDialog::Draw(const EditorContext& ctx)
{
    DrawPopup("Load Scene Fragment", ctx.dimensions, [&]()
    {
        ImGui::RadioButton("Overlay on current scene", &m_openType, OpenOverlayed);
        ImGui::RadioButton("Open in new scene", &m_openType, OpenInNewScene);
        SeparatorSpaced();
        InputText(m_fileName, "filename");
        if (ImGui::Button("load"))
        {
            if (m_openType == OpenInNewScene && std::filesystem::is_regular_file(m_fileName))
            {
                m_ncScene->Queue(std::make_unique<SandboxScene>(ctx.objectBucket, ctx.editorCamera, m_fileName));
                m_ncScene->ScheduleTransition();
                ClosePopup();
            }
            else if (auto file = std::ifstream{m_fileName, std::ios::binary})
            {
                LoadSceneFragment(file, m_world, ctx.modules);
                ClosePopup();
            }
            else
            {
                m_errorText = fmt::format("Error opening file '{}'.", m_fileName);
            }
        }

        ImGui::TextWrapped("%s", m_errorText.c_str());
    });
}
} // namespace nc::ui::editor
