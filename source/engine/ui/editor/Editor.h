#pragma once

#include "EditorUI.h"

#include "ecs/Registry.h"

namespace nc::ui::editor
{
class Editor
{
    public:
        Editor();
        void Frame(float* dt, Registry* registry);

    private:
        EditorUI m_ui;
        bool m_open = false;
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
