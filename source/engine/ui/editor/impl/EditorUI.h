#pragma once

#include "windows/FpsOverlay.h"
#include "windows/Inspector.h"
#include "windows/ModalDialog.h"
#include "windows/SceneGraph.h"

#include "ncengine/ecs/Ecs.h"

#include <memory>
#include <vector>

namespace nc
{
class Scene;

namespace asset
{
class NcAsset;
} // namespace asset

namespace ui::editor
{
struct EditorHotkeys;

class EditorUI
{
    public:
        explicit EditorUI(ecs::Ecs world, std::function<void(std::unique_ptr<Scene>)> changeScene);

        void Draw(const EditorHotkeys& hotkeys, ecs::Ecs world, asset::NcAsset& ncAsset);

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
} // namespace ui::editor
} // namespace nc
