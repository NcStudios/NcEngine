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
class SandboxScene : public Scene
{
    public:
        void Load(Registry* registry, ModuleProvider modules) override
        {
            auto cameraHandle = registry->Add<Entity>(
            {
                .position = Vector3{0.0f, 6.1f, -6.5f},
                .rotation = Quaternion::FromEulerAngles(0.7f, 0.0f, 0.0f),
                .tag = "Scene Camera",
                .flags = Entity::Flags::NoSerialize
            });

            auto camera = registry->Add<graphics::SceneNavigationCamera>(cameraHandle);
            registry->Add<FrameLogic>(cameraHandle, InvokeFreeComponent<graphics::SceneNavigationCamera>{});
            modules.Get<graphics::NcGraphics>()->SetCamera(camera);
        }
};

class DialogBase
{
    public:
        auto IsOpen() const noexcept { return m_isOpen; }

    protected:
        auto OpenPopup() { m_isOpen = true; }
        auto ClosePopup() { m_isOpen = false; ImGui::CloseCurrentPopup(); }
        auto DrawPopup(const char* label, auto&& func)
        {
            ImGui::OpenPopup(label);
            ImGui::SetNextWindowSize(ImVec2{400.0f, 100.0f}, ImGuiCond_Once);
            if (ImGui::BeginPopupModal(label, &m_isOpen))
            {
                func();
                ImGui::EndPopup();
            }
        }

    private:
        bool m_isOpen = false;
};

class NewSceneDialog : public DialogBase
{
    public:
        void Open() { OpenPopup(); }
        void Draw(NcEngine* engine)
        {
            DrawPopup("Open Sandbox Scene", [&]()
            {
                ImGui::Text("Are you sure you want to open a new sandbox scene?");
                if (ImGui::Button("Create Sandbox"))
                {
                    engine->QueueSceneChange(std::make_unique<SandboxScene>());
                    ClosePopup();
                }
            });
        }

    private:
        NcEngine* m_engine;
};

class SaveSceneDialog : public DialogBase
{
    public:
        void Open(asset::AssetMap assets)
        {
            OpenPopup();
            m_fileName.clear();
            m_assets = std::move(assets);
        }

        void Draw(ecs::Ecs world)
        {
            DrawPopup("Save Scene Fragment", [&]()
            {
                InputText(m_fileName, "name");
                if (ImGui::Button("save"))
                {
                    // Workaround: Allow editor + game to easily exclude entities from serialization.
                    //             We'll most likely want to add as a permanent solution.
                    static constexpr auto entityFilter = [](Entity entity){ return entity.IsSerializable(); };
                    const auto fileName = m_fileName.empty() ? std::string{"unnamed_fragment"} : m_fileName;
                    if (auto fragmentFile = std::ofstream{fileName, std::ios::binary | std::ios::trunc})
                    {
                        SaveSceneFragment(fragmentFile, world, m_assets, entityFilter);
                        ClosePopup();
                    }
                    else
                    {
                        m_fileName = fmt::format("error saving file '{}'", m_fileName); // idk, don't have immediate error reporting
                    }
                }
            });
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
            OpenPopup();
            m_fileName.clear();
        }

        void Draw(ecs::Ecs world, asset::NcAsset& assetModule)
        {
            DrawPopup("Load Scene Fragment", [&]()
            {
                InputText(m_fileName, "name");
                if (ImGui::Button("load"))
                {
                    if (auto fragmentFile = std::ifstream{m_fileName, std::ios::binary})
                    {
                        LoadSceneFragment(fragmentFile, world, assetModule);
                        ClosePopup();
                    }
                    else
                    {
                        m_fileName = fmt::format("error opening file '{}'", m_fileName);
                    }
                }
            });
        }

    private:
        std::string m_fileName;
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
            else if (input::KeyDown(hotkey::NewScene))
                m_newSceneDialog.Open();
            else if(input::KeyDown(hotkey::SaveScene))
                m_saveSceneDialog.Open(assetModule.GetLoadedAssets());
            else if(input::KeyDown(hotkey::LoadScene))
                m_loadSceneDialog.Open();

            if (m_newSceneDialog.IsOpen())
                m_newSceneDialog.Draw(m_engine);
            else if (m_saveSceneDialog.IsOpen())
                m_saveSceneDialog.Draw(world);
            else if (m_loadSceneDialog.IsOpen())
                m_loadSceneDialog.Draw(world, assetModule);

            m_ui.Draw(world);

            // Workaround: PointLight creation crashes b/c registry + shader resource are out of sync
            //             until next frame. Hacky solution is to make a duplicate sync call here.
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
