#pragma once

#include "ncengine/asset/NcAsset.h"
#include "ncengine/ecs/Ecs.h"
#include "ncengine/scene/NcScene.h"
#include "ncengine/ui/ImGuiUtility.h"

namespace nc
{
class Scene;

namespace ui::editor
{
class ModalDialog
{
    public:
        auto IsOpen() const noexcept -> bool
        {
            return m_isOpen;
        }

    protected:
        explicit ModalDialog(Vector2 size) noexcept
            : m_size{size.x, size.y} {}

        auto OpenPopup()
        {
            m_isOpen = true;
        }

        void ClosePopup()
        {
            m_isOpen = false;
            ImGui::CloseCurrentPopup();
        }

        void DrawPopup(const char* label, const ImVec2& dimensions, auto&& func)
        {
            ImGui::OpenPopup(label);
            ImGui::SetNextWindowSize(m_size, ImGuiCond_Once);
            ImGui::SetNextWindowPos(dimensions * 0.5f, ImGuiCond_Once, ImVec2{0.5f, 0.5f});
            if (ImGui::BeginPopupModal(label, &m_isOpen))
            {
                func();
                ImGui::EndPopup();
            }
        }

    private:
        ImVec2 m_size;
        bool m_isOpen = false;
};

class NewSceneDialog : public ModalDialog
{
    static constexpr auto DialogSize = Vector2{370.0f, 128.0f};

    public:
        explicit NewSceneDialog(NcScene* ncScene);

        void Open();
        void Draw(const ImVec2& dimensions);

    private:
        NcScene* m_ncScene;
};

class SaveSceneDialog : public ModalDialog
{
    static constexpr auto DialogSize = Vector2{350.0f, 400.0f};
    static constexpr auto LayerInclusionAll = 0;
    static constexpr auto LayerInclusionFilter = 1;

    public:
        explicit SaveSceneDialog(ecs::Ecs world);

        void Open(asset::AssetMap assets);
        void Draw(const ImVec2& dimensions);

    private:
        std::string m_fileName;
        std::string m_errorText;
        ecs::Ecs m_world;
        asset::AssetMap m_assets;

        bool m_includeAssets = false;
        bool m_includePersistent = false;
        int m_layerInclusionType = LayerInclusionAll;
        std::vector<uint8_t> m_includedLayers;
        ptrdiff_t m_selectedLayer = 0;
        bool m_backup = true;

        void DrawLayerFilterListBox();
        void AddLayer();
        void RemoveSelectedLayer();
        void EnableAllLayers();
        void DisableAllLayers();
        void OnSave();
        auto SelectFilter() -> std::function<bool(Entity)>;
};

class LoadSceneDialog : public ModalDialog
{
    static constexpr auto DialogSize = Vector2{310.0f, 170.0f};
    static constexpr auto OpenOverlayed = 0;
    static constexpr auto OpenInNewScene = 1;

    public:
        explicit LoadSceneDialog(ecs::Ecs world, NcScene* ncScene);

        void Open(asset::NcAsset* ncAsset);
        void Draw(const ImVec2& dimensions);

    private:
        std::string m_fileName;
        std::string m_errorText;
        ecs::Ecs m_world;
        NcScene* m_ncScene;
        asset::NcAsset* m_ncAsset = nullptr;

        int m_openType = OpenOverlayed;
};
} // namespace ui::editor
} // namespace nc
