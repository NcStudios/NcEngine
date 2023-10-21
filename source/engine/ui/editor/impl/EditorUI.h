#pragma once

#include "windows/Inspector.h"
#include "windows/SceneGraph.h"

namespace nc
{
class Registry;

namespace ui::editor
{
class EditorUI
{
    public:
        void Draw(Registry* registry);

    private:
        SceneGraph m_sceneGraph;
        Inspector m_inspector;
        bool m_open = false;
};
} // namespace ui::editor
} // namespace nc
