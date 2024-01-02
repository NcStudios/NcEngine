#include "ui/editor/Editor.h"
#include "EditorUI.h"
#include "input/Input.h"
#include "window/Window.h"



#include <iostream>
#include <fstream>
#include "ncengine/serialize/SceneSerialization.h"
#include "ncengine/scene/SceneFragment.h"

namespace
{
namespace hotkey
{
constexpr auto Editor = nc::input::KeyCode::Tilde;
} // namespace hotkey
} // anonymous namespace

namespace nc::ui::editor
{
class EditorImpl : public Editor
{
    public:
        void Draw(ecs::Ecs world, asset::NcAsset& assetModule) override
        {
            if(input::KeyDown(hotkey::Editor))
                m_open = !m_open;

            if(!m_open)
                return;

            /** @todo REMOVE BEFORE CHECK IN - for local testing. */
            if (input::KeyDown(input::KeyCode::S))
            {
                std::cerr << "Saving fragment\n";
                auto fragment = SaveSceneFragment(world, assetModule);
                auto file = std::ofstream{"fragment.nca", std::ios::binary | std::ios::trunc};
                Serialize(file, fragment);
            }
            if (input::KeyDown(input::KeyCode::L))
            {
                std::cerr << "Loading fragment\n";
                auto file = std::ifstream{"fragment.nca", std::ios::binary};
                auto fragment = SceneFragment{};
                Deserialize(file, fragment);
                LoadSceneFragment(fragment, world, assetModule);
            }

            m_ui.Draw(world);
        }

    private:
        EditorUI m_ui;
        bool m_open = false;
};

auto BuildEditor() -> std::unique_ptr<Editor>
{
    return std::make_unique<EditorImpl>();
}
} // namespace nc::ui::editor
