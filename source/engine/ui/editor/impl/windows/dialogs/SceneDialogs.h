#pragma once

#include "ModalDialog.h"
#include "ui/Editor.h"

#include "ncengine/asset/NcAsset.h"
#include "ncengine/ecs/Ecs.h"
#include "ncengine/scene/NcScene.h"

namespace nc::ui::editor
{
class NewSceneDialog : public ModalDialog
{
    static constexpr auto DialogSize = Vector2{370.0f, 128.0f};

    public:
        explicit NewSceneDialog(NcScene* ncScene) noexcept
            : ModalDialog{DialogSize}, m_ncScene{ncScene} {}

        void Open()
        {
            OpenPopup();
        }

        void Draw(const EditorContext& ctx);

    private:
        NcScene* m_ncScene;
};

class SaveSceneDialog : public ModalDialog
{
    static constexpr auto DialogSize = Vector2{350.0f, 400.0f};
    static constexpr auto LayerInclusionAll = 0;
    static constexpr auto LayerInclusionFilter = 1;

    public:
        explicit SaveSceneDialog(ecs::Ecs world) noexcept
            : ModalDialog{DialogSize}, m_world{world} {}

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
        explicit LoadSceneDialog(ecs::Ecs world, NcScene* ncScene) noexcept
            : ModalDialog{DialogSize}, m_world{world}, m_ncScene{ncScene} {}

        void Open(asset::NcAsset* ncAsset);
        void Draw(const EditorContext& ctx);

    private:
        std::string m_fileName;
        std::string m_errorText;
        ecs::Ecs m_world;
        NcScene* m_ncScene;
        asset::NcAsset* m_ncAsset = nullptr;

        int m_openType = OpenOverlayed;
};
} // namespace nc::ui::editor
