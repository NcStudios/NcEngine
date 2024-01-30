#pragma once

#include "ncengine/asset/NcAsset.h"
#include "ncengine/ecs/Ecs.h"
#include "ncengine/ui/ImGuiUtility.h"

namespace nc::ui::editor
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

        void DrawPopup(const char* label, auto&& func)
        {
            ImGui::OpenPopup(label);
            ImGui::SetNextWindowSize(m_size, ImGuiCond_Once);
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
    static constexpr auto DialogSize = Vector2{256.0f, 128.0f};

    public:
        explicit NewSceneDialog();

        void Open();
        void Draw();

    private:

};

class SaveSceneDialog : public ModalDialog
{
    static constexpr auto DialogSize = Vector2{400.0f, 300.0f};
    static constexpr auto LayerInclusionAll = 0;
    static constexpr auto LayerInclusionFilter = 1;

    public:
        explicit SaveSceneDialog(ecs::Ecs world);

        void Open(asset::AssetMap assets);
        void Draw();

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
        void EnableAllLayers();
        void DisableAllLayers();
        void OnSave();
};

class LoadSceneDialog : public ModalDialog
{
    static constexpr auto DialogSize = Vector2{300.0f, 128.0f};
    static constexpr auto OpenOverlayed = 0;
    static constexpr auto OpenInNewScene = 1;

    public:
        explicit LoadSceneDialog(ecs::Ecs world);

        void Open(asset::NcAsset* ncAsset);
        void Draw();

    private:
        std::string m_fileName;
        std::string m_errorText;
        ecs::Ecs m_world;
        asset::NcAsset* m_ncAsset = nullptr;

        int m_openType = OpenOverlayed;
};

} // namespace nc::ui::editor
