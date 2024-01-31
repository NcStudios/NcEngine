#pragma once

#include "windows/FpsOverlay.h"
#include "windows/Inspector.h"
#include "windows/ModalDialog.h"
#include "windows/SceneGraph.h"

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

        void Draw(const EditorHotkeys& hotkeys, ecs::Ecs world, ModuleProvider modules);

    private:
        SceneGraph m_sceneGraph;
        Inspector m_inspector;
        bool m_open = false;

        // overlays
        FpsOverlay m_fpsOverlay;

        // dialogs
        NewSceneDialog m_newSceneDialog;
        SaveSceneDialog m_saveSceneDialog;
        LoadSceneDialog m_loadSceneDialog;

        void ProcessInput(const EditorHotkeys& hotkeys, asset::NcAsset& ncAsset);
        void DrawMenu(asset::NcAsset& ncAsset);
        void DrawOverlays(const ImVec2& dimensions);
        void DrawDialogs(const ImVec2& dimensions);
};
} // namespace nc::ui::editor
