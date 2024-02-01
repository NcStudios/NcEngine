#pragma once

#include "ncengine/ecs/Ecs.h"
#include "windows/CreateEntityWindow.h"
#include "windows/Inspector.h"
#include "windows/SceneGraph.h"

namespace nc
{

namespace ui::editor
{
class EditorUI
{
    public:
        void Draw(ecs::Ecs world);

    private:
        SceneGraph m_sceneGraph;
        Inspector m_inspector;
        bool m_open = false;

        CreateEntityWindow m_createEntityWindow;
};
} // namespace ui::editor
} // namespace nc
