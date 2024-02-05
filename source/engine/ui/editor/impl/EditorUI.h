#pragma once

#include "ui/editor/Editor.h"
#include "windows/FpsOverlay.h"
#include "windows/Inspector.h"
#include "windows/SceneGraph.h"
#include "windows/dialogs/CreateEntityDialog.h"
#include "windows/dialogs/SceneDialogs.h"

#include "ncengine/ecs/Ecs.h"
#include "ncengine/module/ModuleProvider.h"

#include <memory>
#include <vector>

namespace nc::ui::editor
{
struct EditorHotkeys;

class EditorUI
{
    public:
        explicit EditorUI(ecs::Ecs world, ModuleProvider modules);

        void Draw(EditorContext& ctx);

    private:
        SceneGraph m_sceneGraph;
        Inspector m_inspector;
        bool m_open = false;

        // overlays
        FpsOverlay m_fpsOverlay;

        // dialogs
        CreateEntityDialog m_createEntityDialog;
        NewSceneDialog m_newSceneDialog;
        SaveSceneDialog m_saveSceneDialog;
        LoadSceneDialog m_loadSceneDialog;

        auto ProcessInput(const EditorHotkeys& hotkeys, asset::NcAsset& ncAsset) -> OpenState;
        void DrawMenu(asset::NcAsset& ncAsset);
        void DrawOverlays(const ImVec2& dimensions);
        void DrawDialogs(const ImVec2& dimensions);
};
} // namespace nc::ui::editor
