#include "ncengine/ui/Editor.h"
#include "ncengine/NcEngine.h"
#include "ncengine/input/Input.h"
#include "ncengine/module/ModuleProvider.h"
#include "ncengine/window/Window.h"
#include "EditorUI.h"

#include "ncengine/ui/ImGuiUtility.h"
#include "ncengine/ecs/InvokeFreeComponent.h"
#include "ncengine/graphics/NcGraphics.h"
#include "ncengine/graphics/SceneNavigationCamera.h"
#include "ncengine/serialize/SceneSerialization.h"
#include <fstream>

namespace
{
namespace hotkey
{
constexpr auto Editor = nc::input::KeyCode::Tilde;
constexpr auto NewScene = nc::input::KeyCode::F1;
constexpr auto SaveScene = nc::input::KeyCode::F2;
constexpr auto LoadScene = nc::input::KeyCode::F3;
} // namespace hotkey
} // anonymous namespace

namespace nc::ui::editor
{
class DummyScene : public Scene
{
    public:
        void Load(Registry* registry, ModuleProvider modules) override
        {
            auto cameraHandle = registry->Add<Entity>({.position = Vector3{0.0f, 6.1f, -6.5f}, .rotation = Quaternion::FromEulerAngles(0.7f, 0.0f, 0.0f), .tag = "Scene Camera"});
            auto camera = registry->Add<graphics::SceneNavigationCamera>(cameraHandle);
            registry->Add<FrameLogic>(cameraHandle, InvokeFreeComponent<graphics::SceneNavigationCamera>{});
            modules.Get<graphics::NcGraphics>()->SetCamera(camera);
        }
};

class DialogBase
{
    public:
        auto IsOpen() const noexcept { return m_isOpen; }
        auto ToggleOpen(bool state) noexcept { m_isOpen = state; }
        auto GetOpen() -> bool* { return &m_isOpen; }

    private:
        bool m_isOpen = false;
};

class NewSceneDialog : public DialogBase
{
    public:
        void Open()
        {
            ToggleOpen(true);
        }

        void Draw(NcEngine* engine)
        {
            ImGui::OpenPopup("Create New Scene");
            if (ImGui::BeginPopupModal("Create New Scene", GetOpen()))
            {
                ImGui::Text("Are you sure you want to create a new scene?");
                if (ImGui::Button("Create Scene"))
                {
                    engine->QueueSceneChange(std::make_unique<DummyScene>());
                    ToggleOpen(false);
                    ImGui::CloseCurrentPopup();
                }

                ImGui::EndPopup();
            }
        }

    private:

        NcEngine* m_engine;
};

class SaveSceneDialog : public DialogBase
{
    public:
        void Open(asset::AssetMap assets)
        {
            ToggleOpen(true);
            m_fileName.clear();
            m_assets = std::move(assets);
        }

        void Draw(ecs::Ecs world)
        {
            ImGui::OpenPopup("Save Scene Fragment");
            if (ImGui::BeginPopupModal("Save Scene Fragment", GetOpen()))
            {
                InputText(m_fileName, "fragment name");
                if (ImGui::Button("save as"))
                {
                    // TODO: filter by layer
                    static constexpr auto persistentFilter = [](Entity entity) { return !entity.IsPersistent(); };
                    const auto fileName = m_fileName.empty() ? std::string{"unnamed_fragment"} : m_fileName;
                    if (auto fragmentFile = std::ofstream{fileName, std::ios::binary | std::ios::trunc})
                    {
                        SaveSceneFragment(fragmentFile, world, m_assets, persistentFilter);
                        ToggleOpen(false);
                        ImGui::CloseCurrentPopup();
                    }
                    else
                    {
                        m_fileName = fmt::format("error opening file '{}'", m_fileName); // idk, don't have immediate error reporting
                    }
                }

                ImGui::EndPopup();
            }
        }

    private:
        std::string m_fileName;
        asset::AssetMap m_assets;
};

class LoadSceneDialog : public DialogBase
{
    public:
        void Open()
        {
            ToggleOpen(true);
            m_fileName.clear();
        }

        void Draw(ecs::Ecs world, asset::NcAsset& assetModule)
        {
            ImGui::OpenPopup("Load Scene Fragment");
            if (ImGui::BeginPopupModal("Load Scene Fragment", GetOpen()))
            {
                InputText(m_fileName, "fragment name");
                if (ImGui::Button("load"))
                {
                    if (auto fragmentFile = std::ifstream{m_fileName, std::ios::binary})
                    {
                        LoadSceneFragment(fragmentFile, world, assetModule);
                        ToggleOpen(false);
                        ImGui::CloseCurrentPopup();
                    }
                    else
                    {
                        m_fileName = fmt::format("error opening file '{}'", m_fileName);
                    }
                }

                ImGui::EndPopup();
            }
        }

    private:
        bool m_isOpen = false;
        std::string m_fileName;
        asset::AssetMap m_assets;
};

class EditorImpl : public Editor
{
    public:
        explicit EditorImpl(NcEngine* engine)
            : m_ui{}, m_engine{engine} {}

        void Draw(ecs::Ecs world, asset::NcAsset& assetModule) override
        {
            if(input::KeyDown(hotkey::Editor))
                m_open = !m_open;

            if(!m_open)
                return;

            if (input::KeyDown(hotkey::NewScene))
                m_newSceneDialog.Open();
            else if(input::KeyDown(hotkey::SaveScene))
                m_saveSceneDialog.Open(assetModule.GetLoadedAssets());
            else if(input::KeyDown(hotkey::LoadScene))
                m_loadSceneDialog.Open();

            if (m_newSceneDialog.IsOpen())
                m_newSceneDialog.Draw(m_engine);

            if (m_saveSceneDialog.IsOpen())
                m_saveSceneDialog.Draw(world);

            if (m_loadSceneDialog.IsOpen())
                m_loadSceneDialog.Draw(world, assetModule);

            m_ui.Draw(world);

            // PointLight workaround
            m_engine->GetRegistry()->CommitStagedChanges();
        }

    private:
        EditorUI m_ui;
        NewSceneDialog m_newSceneDialog;
        SaveSceneDialog m_saveSceneDialog;
        LoadSceneDialog m_loadSceneDialog;
        NcEngine* m_engine;
        bool m_open = false;
};

auto BuildEditor(NcEngine* engine) -> std::unique_ptr<Editor>
{
    return std::make_unique<EditorImpl>(engine);
}
} // namespace nc::ui::editor
