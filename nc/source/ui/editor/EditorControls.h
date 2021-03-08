#pragma once
#ifdef NC_EDITOR_ENABLED
#include "debug/Profiler.h"
#include "Ecs.h"
#include "graphics/d3dresource/GraphicsResourceManager.h"
#include "imgui/imgui.h"

namespace nc::ui::editor::controls
{
    const auto TitleBarHeight = 40.0f;
    const auto DefaultItemWidth = 60.0f;
    const auto SceneGraphPanelWidth = 300;
    const auto UtilitiesPanelHeight = 350;
    const auto GraphSize = ImVec2{128, 32};
    const auto Padding = 4.0f;

    inline void SceneGraphPanel(ecs::EntityMap& entities, float windowHeight);
    inline void UtilitiesPanel(float* dtMult, unsigned drawCallCount, float windowWidth, float windowHeight);
    inline void GraphicsResourcePanel();
    inline void Entity(EntityHandle handle);
    inline void Component(ComponentBase* comp);
    inline void FrameData(float* dtMult, unsigned drawCallCount);
    inline void Profiler();

    /**
     * Scene Graph Controls
     */
    void SceneGraphPanel(ecs::EntityMap& entities, float windowHeight)
    {
        ImGui::SetNextWindowPos({Padding, TitleBarHeight});

        if(ImGui::BeginChild("ScenePanel", {SceneGraphPanelWidth, windowHeight - TitleBarHeight}, true))
        {
            static ImGuiTextFilter filter;
            ImGui::Text("Scene Graph");
            ImGui::Spacing();
            filter.Draw("##filterlabel", 2.0f * DefaultItemWidth);
            ImGui::Separator();
            ImGui::Separator();
            ImGui::Spacing();

            if(ImGui::BeginChild("EntityList"))
            {
                for(auto& [handle, entity] : entities)
                {
                    if(!filter.PassFilter(entity.Tag.c_str()))
                        continue;
                    ImGui::PushID(static_cast<unsigned>(handle));
                    if(ImGui::CollapsingHeader(entity.Tag.c_str()))
                        controls::Entity(handle);
                    ImGui::PopID();
                }
            }
            ImGui::EndChild();
        }
        ImGui::EndChild();
    }

    void Entity(EntityHandle handle)
    {
        auto* entity = GetEntity(handle);
        ImGui::Separator();
        ImGui::Indent();
            ImGui::Text("Handle %d", static_cast<unsigned>(handle));
            ImGui::Text("Static %s", entity->IsStatic ? "True" : "False");
        ImGui::Unindent();

        controls::Component(GetComponent<Transform>(handle));
        controls::Component(GetComponent<NetworkDispatcher>(handle));
        controls::Component(GetComponent<Renderer>(handle));
        if(auto col = GetComponent<Collider>(handle); col)
        {
            // collider model doesn't update/submit unless we tell it to
            col->SetEditorSelection(true);
            controls::Component(col);
        }
        controls::Component(GetComponent<PointLight>(handle));
        for(const auto& comp : entity->GetUserComponents())
            controls::Component(comp);

        ImGui::Separator();
    }

    void Component(ComponentBase* comp)
    {
        if(!comp)
            return;
        ImGui::Separator();
        ImGui::BeginGroup();
            ImGui::Indent();
            ImGui::Spacing();
            comp->EditorGuiElement();
            ImGui::Spacing();
        ImGui::EndGroup();
    }

    /**
     * Utilities Controls
     */
        template<class Func, class ... Args>
    void WrapTabItem(const char* label, Func func, Args&& ... args)
    {
        if(ImGui::BeginTabItem(label))
        {
            if(ImGui::BeginChild("", {0,0}, true))
                func(std::forward<Args>(args)...);
            ImGui::EndChild();
            ImGui::EndTabItem();
        }
    }

    void UtilitiesPanel(float* dtMult, unsigned drawCallCount, float windowWidth, float windowHeight)
    {
        static auto initColumnWidth = false;
        const auto xPos = SceneGraphPanelWidth + 2.0f * Padding;
        auto panelWidth = windowWidth - xPos;
        ImGui::SetNextWindowPos({xPos, windowHeight - UtilitiesPanelHeight});

        if(ImGui::BeginChild("UtilitiesPanel", {panelWidth, UtilitiesPanelHeight}, true))
        {
            ImGui::Columns(2);
            if(!initColumnWidth)
            {
                ImGui::SetColumnWidth(-1, 0.85 * panelWidth);
                initColumnWidth = true;
            }
            if(ImGui::BeginTabBar("UtilitiesLeftTabBar"))
            {
                WrapTabItem("Profiler", Profiler);
                WrapTabItem("Gfx Resources", GraphicsResourcePanel);
                ImGui::EndTabBar();
            }

            ImGui::NextColumn();
            if(ImGui::BeginTabBar("UtilitiesRightTabBar"))
            {
                WrapTabItem("Frame Data", FrameData, dtMult, drawCallCount);
                WrapTabItem("UI Style", ImGui::ShowStyleEditor, nullptr);
                WrapTabItem("UI Metrics", ImGui::ShowMetricsWindow, nullptr);
                ImGui::EndTabBar();
            }
        }
        ImGui::EndChild();
    }

    void FrameData(float* dtMult, unsigned drawCallCount)
    {
        float frameRate = ImGui::GetIO().Framerate;
        ImGui::SetNextItemWidth(DefaultItemWidth);
        ImGui::DragFloat("dtX", dtMult, 0.1f, 0.0f, 5.0f, "%.1f");
        ImGui::Text("%.1f fps", frameRate);
        ImGui::Text("%.1f ms/frame", 1000.0f / frameRate);
        ImGui::Text("%u Draw Calls", drawCallCount);
    }

    void Profiler()
    {
        using nc::debug::profiler::Filter;

        static ImGuiTextFilter textFilter;
        static int filterSelection = 0u;

        textFilter.Draw("Search##telemetry", 128.0f);

        for(auto v : {Filter::All, Filter::Engine, Filter::User})
        {
            ImGui::SameLine();
            ImGui::RadioButton(ToCString(v), &filterSelection, static_cast<int>(v));
        }

        ImGui::Separator();

        if(ImGui::BeginChild("Profiler##child"))
        {
            const auto filterGroup = static_cast<Filter>(filterSelection);
            for(auto& [id, data] : nc::debug::profiler::GetData())
            {
                if(!textFilter.PassFilter(data.functionName.c_str()))
                    continue;
                
                if((filterGroup != Filter::All) && (filterGroup != data.filter))
                    continue;

                ImGui::PushID(id);
                if(ImGui::CollapsingHeader(data.functionName.c_str()))
                {
                    debug::profiler::UpdateHistory(&data);
                    auto [avgCalls, avgTime] = debug::profiler::ComputeAverages(&data);
                    ImGui::Indent();
                        ImGui::Text("   Calls: %.1f", avgCalls);
                        ImGui::SameLine();
                        ImGui::PlotHistogram("##calls", data.callHistory.data(), data.historySize, 0, nullptr, 0.0f, 100.0f, GraphSize);
                        ImGui::SameLine();
                        ImGui::Text("   Time: %.1f", avgTime);
                        ImGui::SameLine();
                        ImGui::PlotHistogram("##time", data.timeHistory.data(), data.historySize, 0, nullptr, 0.0f, 20.0f, GraphSize);
                    ImGui::Unindent();
                }
                ImGui::PopID();
                debug::profiler::Reset(&data);
            }
        }
        ImGui::EndChild();
    }

    void GraphicsResourcePanel()
    {
        static ImGuiTextFilter filter;
        filter.Draw("##gfxFilterId", 128.0f);
        for(auto& [id, res] : graphics::d3dresource::GraphicsResourceManager::Get().m_resources)
        {
            if(filter.PassFilter(id.c_str()))
                ImGui::Text(id.c_str());
        }
    }
} // end namespace nc::ui::editor
#endif