#pragma once
#ifdef NC_EDITOR_ENABLED
#include "Profile.h"
#include "Ecs.h"
#include "graphics/d3dresource/GraphicsResourceManager.h"
#include "imgui/imgui.h"


namespace nc::ui::editor::controls
{
    const auto DefaultItemWidth = 60.0f;
    const auto Padding = 4.0f;
    const auto PanelSize = ImVec2{300, 0};
    const auto SubPanelWidth = PanelSize.x - Padding * 4.0f;

    inline void SceneGraphPanel(ecs::EntityMap& entities);
    inline void UtilitiesPanel(float* dtMult, unsigned drawCallCount);
    inline void GraphicsResourcePanel();
    inline void Entity(EntityHandle handle);
    inline void Component(ComponentBase* comp);
    inline void FrameData(float* dtMult, unsigned drawCallCount);
    inline void Profiler();

    /**
     * Scene Graph Controls
     */
    void SceneGraphPanel(ecs::EntityMap& entities)
    {
        if(ImGui::BeginChild("ScenePanel", PanelSize, true))
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
                auto uidScope = 0u;
                for(auto& [handle, entity] : entities)
                {
                    if(!filter.PassFilter(entity.Tag.c_str()))
                        continue;
                    ImGui::PushID(++uidScope);
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
        ImGui::Separator();
        ImGui::Indent();
            ImGui::Text("Handle %d", static_cast<unsigned>(handle));
        ImGui::Unindent();

        controls::Component(nc::Ecs::GetComponent<nc::Transform>(handle));

        nc::NetworkDispatcher* disp = nc::Ecs::GetComponent<nc::NetworkDispatcher>(handle);
        if(disp) { controls::Component(disp); }

        nc::Renderer* rend = nc::Ecs::GetComponent<nc::Renderer>(handle);
        if(rend) { controls::Component(rend); }

        nc::Collider* col = nc::Ecs::GetComponent<nc::Collider>(handle);
        if(col) { controls::Component(col); }

        nc::PointLight* light = nc::Ecs::GetComponent<PointLight>(handle);
        if(light) { controls::Component(light); }

        for(const auto& comp : nc::Ecs::GetEntity(handle)->GetUserComponents())
        {
            controls::Component(comp.get());
        }

        ImGui::Separator();
    }

    void Component(ComponentBase* comp)
    {
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
    void UtilitiesPanel(float* dtMult, unsigned drawCallCount)
    {
        if(ImGui::BeginChild("UtilityPanel", PanelSize, true))
        {
            ImGui::Text("Utilities");
            ImGui::Separator();
            ImGui::Separator();
            ImGui::Spacing();
            if(ImGui::CollapsingHeader("Frame Data"))
            {
                FrameData(dtMult, drawCallCount);
                ImGui::Separator();
            }
            if(ImGui::CollapsingHeader("Profiling"))
            {
                Profiler();
                ImGui::Separator();
            }
            if(ImGui::CollapsingHeader("ImGui Style Options"))
            {
                ImGui::ShowStyleEditor();
                ImGui::Separator();
            }
        }
        ImGui::EndChild();
    }

    void FrameData(float* dtMult, unsigned drawCallCount)
    {
        ImGui::BeginGroup();
        ImGui::Indent();
            float frameRate = ImGui::GetIO().Framerate;
            ImGui::SetNextItemWidth(DefaultItemWidth);
            ImGui::DragFloat("dtX", dtMult, 0.1f, 0.0f, 5.0f, "%.1f");
            ImGui::Text("%.1f fps", frameRate);
            ImGui::Text("%.1f ms/frame", 1000.0f / frameRate);
            ImGui::Text("%u Draw Calls", drawCallCount);
        ImGui::Unindent();
        ImGui::EndGroup();
    }

    void Profiler()
    {
        using nc::debug::profile::Filter;

        static ImGuiTextFilter textFilter;
        static int filterSelection = 0u;

        ImGui::BeginGroup();
        ImGui::Indent();
            ImGui::RadioButton("All", &filterSelection, static_cast<int>(Filter::All));
            ImGui::SameLine();
            ImGui::RadioButton("Engine", &filterSelection, static_cast<int>(Filter::Engine));
            ImGui::SameLine();
            ImGui::RadioButton("User", &filterSelection, static_cast<int>(Filter::User));
            textFilter.Draw("Search##telemetry", 128.0f);
            ImGui::Separator();
            const auto profileFilter = static_cast<Filter>(filterSelection);

            for(auto& [id, data] : nc::debug::profile::GetData())
            {
                if(!textFilter.PassFilter(data.functionName.c_str()))
                    continue;
                
                if((profileFilter != Filter::All) && (profileFilter != data.filter))
                    continue;

                ImGui::PushID(id);
                if(ImGui::CollapsingHeader(data.functionName.c_str()))
                {
                    data.UpdateHistory();
                    auto [avgCalls, avgTime] = data.GetAverages();
                    ImGui::Indent();
                        ImGui::Text("       Avg. Calls: %.1f", avgCalls);
                        ImGui::PlotHistogram("##calls", data.callHistory.data(), data.historySize, 0, nullptr, 0.0f, 100.0f);
                        ImGui::Text("       Avg. Time: %.1f", avgTime);
                        ImGui::PlotHistogram("##time", data.timeHistory.data(), data.historySize, 0, nullptr, 0.0f, 20.0f);
                    ImGui::Unindent();
                }
                ImGui::PopID();
                data.Reset();
            }
        ImGui::Unindent();
        ImGui::EndGroup();
    }

    void GraphicsResourcePanel()
    {
        static ImGuiTextFilter filter;

        if(ImGui::BeginChild("GraphicsResourcePanel", {500, 200}, true, ImGuiWindowFlags_HorizontalScrollbar))
        {
            ImGui::Text("Graphics Resources");
            filter.Draw("##gfxFilterId", 128.0f);
            for(auto& [id, res] : graphics::d3dresource::GraphicsResourceManager::Get().m_resources)
            {
                if(filter.PassFilter(id.c_str()))
                    ImGui::Text(id.c_str());
            }
        }
        ImGui::EndChild();
    }
} // end namespace nc::ui::editor
#endif