#pragma once

#include "ecs/Registry.h"

namespace nc::ui::editor
{
class Editor
{
    public:
        Editor();
        void Frame(float* dt, Registry* registry);

    private:
        bool m_openState_Editor;
        bool m_openState_UtilitiesPanel;

        void DrawMenu();
};

// Provide stubs if editor is disabled
// TODO: #427 Make a proper interface + factory for editor
#ifndef NC_EDITOR_ENABLED
Editor::Editor()
{
}

void Editor::Frame(float*, Registry*)
{
}
#endif
} // namespace nc::ui::editor
