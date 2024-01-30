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
        explicit EditorUI(ecs::Ecs world);

        void Draw(const EditorHotkeys& hotkeys, ecs::Ecs world, asset::NcAsset& ncAsset);

    private:
        SceneGraph m_sceneGraph;
        Inspector m_inspector;
        bool m_open = false;

        FpsOverlay m_fpsOverlay;

        // dialogs
        NewSceneDialog m_newSceneDialog;
        SaveSceneDialog m_saveSceneDialog;
        LoadSceneDialog m_loadSceneDialog;
};
} // namespace ui::editor
} // namespace nc
