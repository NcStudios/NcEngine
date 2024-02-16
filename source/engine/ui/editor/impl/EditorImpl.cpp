#include "ui/editor/Editor.h"
#include "EditorCamera.h"
#include "EditorUI.h"
#include "ncengine/ecs/Logic.h"
#include "ncengine/ecs/InvokeFreeComponent.h"
#include "ncengine/input/Input.h"
#include "ncengine/window/Window.h"

namespace
{
auto BuildEditorObjects(nc::ecs::Ecs world, nc::ModuleProvider modules, nc::input::KeyCode cameraHotkey) -> std::pair<nc::Entity, nc::Entity>
{
    // Parent Entity for all Editor objects
    const auto bucket = world.Emplace<nc::Entity>({
        .tag = "[Editor]",
        .flags = nc::ui::editor::EditorObjectFlags
    });

    // Editor camera
    const auto camera = world.Emplace<nc::Entity>({
        .position = nc::Vector3{0.0f, 8.0f, -10.0f},
        .rotation = nc::Quaternion::FromEulerAngles(0.7f, 0.0f, 0.0f),
        .parent = bucket,
        .tag = "EditorCamera",
        .flags = nc::ui::editor::EditorObjectFlags
    });

    world.Emplace<nc::ui::editor::EditorCamera>(camera, modules, cameraHotkey);
    world.Emplace<nc::FrameLogic>(camera, nc::InvokeFreeComponent<nc::ui::editor::EditorCamera>{});
    return std::pair{bucket, camera};
}

auto BuildContext(nc::ecs::Ecs world,
                  nc::ModuleProvider modules,
                  nc::SystemEvents& events,
                  nc::ui::editor::EditorHotkeys hotkeys) -> nc::ui::editor::EditorContext
{
    const auto [bucket, camera] = ::BuildEditorObjects(world, modules, hotkeys.toggleEditorCamera);
    return nc::ui::editor::EditorContext{
        .world = world,
        .modules = modules,
        .events = &events,
        .selectedEntity = nc::Entity::Null(),
        .openState = nc::ui::editor::OpenState::ClosePersisted,
        .dimensions = ImVec2{},
        .objectBucket = bucket,
        .editorCamera = camera,
        .hotkeys = hotkeys
    };
}
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
                .tag = "Camera [sandbox]",
                .flags = Entity::Flags::NoSerialize
            });

            auto camera = registry->Add<graphics::SceneNavigationCamera>(cameraHandle);
            registry->Add<FrameLogic>(cameraHandle, InvokeFreeComponent<graphics::SceneNavigationCamera>{});
            modules.Get<graphics::NcGraphics>()->SetCamera(camera);

            auto light = registry->Add<Entity>(
            {
                .position = Vector3{1.20484f, 9.4f, -8.48875f},
                .tag = "Light [sandbox]",
                .flags = Entity::Flags::NoSerialize
            });

            registry->Add<graphics::PointLight>(light, Vector3{1.0f, 0.957f, 0.725f}, Vector3{1.0f, 0.957f, 0.725f}, 600.0f);
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
        explicit EditorImpl(ecs::Ecs world, ModuleProvider modules, SystemEvents& events, const EditorHotkeys& hotkeys)
            : Editor{::BuildContext(world, modules, events, hotkeys)},
              m_ui{m_ctx}
        {
        }

        void Draw(ecs::Ecs) override
        {
            m_ui.Draw(m_ctx);
        }

    private:
        EditorUI m_ui;
};

auto BuildEditor(ecs::Ecs world,
                 ModuleProvider modules,
                 SystemEvents& events,
                 const EditorHotkeys& hotkeys) -> std::unique_ptr<Editor>
{
    return std::make_unique<EditorImpl>(world, modules, events, hotkeys);
}
} // namespace nc::ui::editor
