#include "ui/editor/Editor.h"
#include "EditorUI.h"
#include "input/Input.h"
#include "window/Window.h"

namespace nc::ui::editor
{
class EditorImpl : public Editor
{
    public:
        explicit EditorImpl(ecs::Ecs world, ModuleProvider modules, const EditorHotkeys& hotkeys)
            : Editor{hotkeys},
              m_modules{modules},
              m_ui{world, modules}
        {
        }

        void Draw(ecs::Ecs world) override
        {
            m_ui.Draw(GetHotkeys(), world, m_modules);
        }

    private:
        ModuleProvider m_modules;
        EditorUI m_ui;
};

auto BuildEditor(ecs::Ecs world,
                 ModuleProvider modules,
                 const EditorHotkeys& hotkeys) -> std::unique_ptr<Editor>
{
    return std::make_unique<EditorImpl>(world, modules, hotkeys);
}
} // namespace nc::ui::editor
