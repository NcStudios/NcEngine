#include "SampleUI.h"
#include "Window.h"
#include "Input.h"
#include "config/Version.h"
#include "UIStyle.h"
#include "imgui/imgui.h"

#include "scenes/Worms.h"
#include "scenes/ClickEvents.h"
#include "scenes/SpawnTest.h"
#include "scenes/CollisionBenchmark.h"
#include "scenes/CollisionEvents.h"
#include "scenes/JointsTest.h"
#include "scenes/RenderingBenchmark.h"
#include "scenes/JareTestScene.h"
#include "scenes/SolarSystem.h"

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
    SampleUI::SampleUI(SceneModule* sceneModule, GameLog* gameLog, std::function<void()> widgetCallback)
        : m_sceneModule{ sceneModule },
        m_gameLog{ gameLog },
        m_widgetCallback{ widgetCallback },
        m_windowDimensions{ window::GetDimensions() }
    {
        SetImGuiStyle();
        window::RegisterOnResizeReceiver(this);
    }

    SampleUI::~SampleUI() noexcept
    {
        window::UnregisterOnResizeReceiver(this);
    }

    void SampleUI::Draw()
    {
        ImGui::SetNextWindowPos({ 0, m_windowDimensions.y - PanelHeight });
        ImGui::SetNextWindowSize({ m_windowDimensions.x, PanelHeight });

        if (ImGui::Begin("SampleUI", nullptr, WindowFlags))
        {
            auto columnCount = m_gameLog ? 3 : 2;
            ImGui::Columns(columnCount);
            m_widgetCallback ? m_widgetCallback() : DrawDefaultWidget();
            if (m_gameLog)
            {
                ImGui::NextColumn();
                DrawLog();
            }
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
        ImGui::Text("Log");
        ImGui::SameLine(columnWidth - 50);
        if (ImGui::Button("Clear", { 42, 18 }))
            m_gameLog->Clear();
        ImGui::SameLine(columnWidth - 130);
        ImGui::SetNextItemWidth(70);
        if (ImGui::InputInt("##logcount", &ItemCount, 1, 5))
        {
            ItemCount = nc::math::Clamp(ItemCount, 0, 1000); //for sanity, since Dear ImGui doesn't deal with unsigned
            m_gameLog->SetItemCount(ItemCount);
        }

        if (ImGui::BeginChild("LogPanel", { 0,0 }, true))
        {
            for (auto& item : m_gameLog->GetItems())
                ImGui::Text(item.c_str());
        } ImGui::EndChild();
    }

    void SampleUI::DrawSceneList()
    {
        ImGui::Text("Scenes");
        if (ImGui::BeginChild("SceneList", { 0,0 }, true))
        {
            auto buttonSize = ImVec2{ ImGui::GetWindowWidth() - 20, 18 };
            if (ImGui::Button("Worms", buttonSize))
                m_sceneModule->ChangeScene(std::make_unique<Worms>());

            if (ImGui::Button("Click Events", buttonSize))
                m_sceneModule->ChangeScene(std::make_unique<ClickEvents>());

            if (ImGui::Button("Collision Events", buttonSize))
                m_sceneModule->ChangeScene(std::make_unique<CollisionEvents>());

            if (ImGui::Button("Joints Test", buttonSize))
                m_sceneModule->ChangeScene(std::make_unique<JointsTest>());

            if (ImGui::Button("Spawn Test", buttonSize))
                m_sceneModule->ChangeScene(std::make_unique<SpawnTest>());

            if (ImGui::Button("Rendering Benchmark", buttonSize))
                m_sceneModule->ChangeScene(std::make_unique<RenderingBenchmark>());

            if (ImGui::Button("Collision Benchmark", buttonSize))
                m_sceneModule->ChangeScene(std::make_unique<CollisionBenchmark>());

            if (ImGui::Button("Jare Test", buttonSize))
                m_sceneModule->ChangeScene(std::make_unique<JareTestScene>());

            if (ImGui::Button("Solar System", buttonSize))
                m_sceneModule->ChangeScene(std::make_unique<SolarSystem>());
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
} //end namespace project::ui