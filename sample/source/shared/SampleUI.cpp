#include "SampleUI.h"
#include "scenes/Worms.h"
#include "scenes/ClickEvents.h"
#include "scenes/SpawnTest.h"
#include "scenes/CollisionBenchmark.h"
#include "scenes/CollisionEvents.h"
#include "scenes/JointsTest.h"
#include "scenes/RenderingBenchmark.h"
#include "scenes/JareTestScene.h"

#include "ncengine/config/Version.h"
#include "ncengine/graphics/NcGraphics.h"
#include "ncengine/input/Input.h"
#include "ncengine/ui/ImGuiStyle.h"
#include "ncengine/window/Window.h"
#include "imgui/imgui.h"

namespace
{
    constexpr auto PanelHeight = 200u;
    constexpr auto WindowFlags = ImGuiWindowFlags_NoCollapse |
						         ImGuiWindowFlags_NoTitleBar |
						         ImGuiWindowFlags_NoResize;

    void Spacing(unsigned count)
    {
        while (count--)
            ImGui::Spacing();
    }
}

namespace nc::sample
{
    auto InitializeSampleUI(NcEngine* engine) -> std::unique_ptr<SampleUI>
    {
        auto ncScene = engine->GetModuleRegistry()->Get<NcScene>();
        auto ui = std::make_unique<SampleUI>(ncScene);
        auto* graphics = engine->GetModuleRegistry()->Get<graphics::NcGraphics>();
        graphics->SetUi(ui.get());
        return ui;
    }

    SampleUI::SampleUI(NcScene* ncScene)
        : m_ncScene{ncScene},
          m_gameLog{},
          m_widgetCallback{},
          m_windowDimensions{window::GetDimensions()},
          m_screenExtent{window::GetScreenExtent()}
    {
        ui::SetDefaultUIStyle();
        window::RegisterOnResizeReceiver(this);
    }

    SampleUI::~SampleUI() noexcept
    {
        window::UnregisterOnResizeReceiver(this);
    }

    void SampleUI::Draw()
    {
        ImGui::SetNextWindowPos({ (m_windowDimensions.x-m_screenExtent.x)/2, m_windowDimensions.y - PanelHeight });
        ImGui::SetNextWindowSize({ m_screenExtent.x, PanelHeight });

        if (ImGui::Begin("SampleUI", nullptr, WindowFlags))
        {
            ImGui::Columns(3);
            m_widgetCallback ? m_widgetCallback() : DrawDefaultWidget();
            ImGui::NextColumn();
            DrawLog();
            ImGui::NextColumn();
            DrawSceneList();
        } ImGui::End();
    }

    void SampleUI::DrawDefaultWidget()
    {
        if (ImGui::BeginChild("DefaultWidget", { 0,0 }, true))
        {
            ImGui::Text("NcEngine Samples");
            ImGui::Text("version");
            ImGui::SameLine();
            ImGui::Text(NC_PROJECT_VERSION);
            Spacing(8);
            ImGui::Text("Press \"~\" to open the editor.");
            Spacing(8);
            ImGui::Text("Provide a callback to draw here.");
        } ImGui::EndChild();
    }

    void SampleUI::DrawLog()
    {
        auto columnWidth = ImGui::GetColumnWidth();
        static int ItemCount = GameLog::DefaultItemCount;
        ImGui::Text("%s", "Log");
        ImGui::SameLine(columnWidth - 50);
        if (ImGui::Button("Clear", { 42, 18 }))
            m_gameLog.Clear();
        ImGui::SameLine(columnWidth - 130);
        ImGui::SetNextItemWidth(70);
        if (ImGui::InputInt("##logcount", &ItemCount, 1, 5))
        {
            ItemCount = nc::Clamp(ItemCount, 0, 1000); //for sanity, since Dear ImGui doesn't deal with unsigned
            m_gameLog.SetItemCount(ItemCount);
        }

        if (ImGui::BeginChild("LogPanel", { 0,0 }, true))
        {
            for (auto& item : m_gameLog.GetItems())
                ImGui::Text("%s", item.c_str());
        } ImGui::EndChild();
    }

    void SampleUI::DrawSceneList()
    {
        ImGui::Text("Scenes");
        if (ImGui::BeginChild("SceneList", { 0,0 }, true))
        {
            auto buttonSize = ImVec2{ ImGui::GetWindowWidth() - 20, 18 };
            if (ImGui::Button("Worms", buttonSize))
            {
                m_ncScene->Queue(std::make_unique<Worms>(this));
                m_ncScene->ScheduleTransition();
            }

            if (ImGui::Button("Click Events", buttonSize))
            {
                m_ncScene->Queue(std::make_unique<ClickEvents>(this));
                m_ncScene->ScheduleTransition();
            }

            if (ImGui::Button("Collision Events", buttonSize))
            {
                m_ncScene->Queue(std::make_unique<CollisionEvents>(this));
                m_ncScene->ScheduleTransition();
            }

            if (ImGui::Button("Joints Test", buttonSize))
            {
                m_ncScene->Queue(std::make_unique<JointsTest>(this));
                m_ncScene->ScheduleTransition();
            }

            if (ImGui::Button("Spawn Test", buttonSize))
            {
                m_ncScene->Queue(std::make_unique<SpawnTest>(this));
                m_ncScene->ScheduleTransition();
            }

            if (ImGui::Button("Rendering Benchmark", buttonSize))
            {
                m_ncScene->Queue(std::make_unique<RenderingBenchmark>(this));
                m_ncScene->ScheduleTransition();
            }

            if (ImGui::Button("Collision Benchmark", buttonSize))
            {
                m_ncScene->Queue(std::make_unique<CollisionBenchmark>(this));
                m_ncScene->ScheduleTransition();
            }

            if (ImGui::Button("Jare Test", buttonSize))
            {
                m_ncScene->Queue(std::make_unique<JareTestScene>(this));
                m_ncScene->ScheduleTransition();
            }
        } ImGui::EndChild();
    }

    bool SampleUI::IsHovered()
    {
        return ImGui::GetIO().WantCaptureMouse;
    }

    void SampleUI::OnResize(nc::Vector2 dimensions)
    {
        m_windowDimensions = dimensions;
    }

    void SampleUI::SetWidgetCallback(std::function<void()> func)
    {
        m_widgetCallback = std::move(func);
    }
} //end namespace project::ui
