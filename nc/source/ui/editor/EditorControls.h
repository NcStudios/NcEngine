#pragma once
#ifdef NC_EDITOR_ENABLED
#include "debug/Profiler.h"
#include "Ecs.h"
#include "ecs/EntityComponentSystem.h"
#include "ecs/ColliderSystem.h"
#include "graphics/d3dresource/GraphicsResourceManager.h"
#include "graphics/vulkan/resources/ResourceManager.h"
#include "imgui/imgui.h"

namespace nc::ui::editor::controls
{
    auto SelectedEntity = HandleTraits::NullHandle;
    const auto TitleBarHeight = 40.0f;
    const auto DefaultItemWidth = 60.0f;
    const auto SceneGraphPanelWidth = 300;
    const auto UtilitiesPanelHeight = 350;
    const auto GraphSize = ImVec2{128, 32};
    const auto Padding = 4.0f;

    inline void SceneGraphPanel(std::span<Entity*> entities, float windowHeight);
    inline void SceneGraphNode(Entity* entity, Transform* transform);
    inline void EntityPanel(EntityHandle handle);
    inline void Component(ComponentBase* comp);
    inline void UtilitiesPanel(float* dtMult, ecs::registry_type* registry, unsigned drawCallCount, float windowWidth, float windowHeight);
    inline void GraphicsResourcePanel();
    inline void FrameData(float* dtMult, unsigned drawCallCount);
    inline void Profiler();
    inline void ComponentSystems(ecs::registry_type* registry);

    /**
     * Scene Graph Controls
     */
    void SceneGraphPanel(std::span<Entity*> entities, float windowHeight)
    {
        ImGui::SetNextWindowPos({Padding, TitleBarHeight});
        auto sceneGraphHeight = windowHeight - TitleBarHeight;

        if(ImGui::BeginChild("ScenePanel", {SceneGraphPanelWidth, sceneGraphHeight}, true))
        {
            static ImGuiTextFilter filter;
            ImGui::Text("Scene Graph");
            ImGui::Spacing();
            filter.Draw("##filterlabel", 2.0f * DefaultItemWidth);
            ImGui::Separator();
            ImGui::Separator();
            ImGui::Spacing();

            if(ImGui::BeginChild("EntityList", {0, sceneGraphHeight / 2}, true))
            {
                for(auto* entity : entities)
                {
                    auto* transform = GetComponent<Transform>(entity->Handle);
                    if(transform->GetParent().Valid()) // only draw root nodes
                        continue;

                    if(!filter.PassFilter(entity->Tag.c_str()))
                        continue;
                    
                    SceneGraphNode(entity, transform);
                }
            } ImGui::EndChild();

            if(ImGui::BeginChild("EntityPanel", {0,0}, true))
            {
                if(SelectedEntity != HandleTraits::NullHandle)
                    controls::EntityPanel(static_cast<EntityHandle>(SelectedEntity));

            } ImGui::EndChild();

        } ImGui::EndChild();
    }

    void SceneGraphNode(Entity* entity, Transform* transform)
    {
        auto handleValue = static_cast<HandleTraits::handle_type>(entity->Handle);
        ImGui::PushID(handleValue);

        auto flags = 0;
        if(SelectedEntity == handleValue)
            flags = flags | ImGuiTreeNodeFlags_Framed;

        auto open = ImGui::TreeNodeEx(entity->Tag.c_str(), flags);
        if(ImGui::IsItemClicked())
            SelectedEntity = handleValue;
        
        if(open)
        {
            for(auto child : transform->GetChildren())
                SceneGraphNode(GetEntity(child), GetComponent<Transform>(child));

            ImGui::TreePop();
        }

        ImGui::PopID();
    }

    void EntityPanel(EntityHandle handle)
    {
        auto* entity = GetEntity(handle);

        if(!entity) // entity may have been deleted
        {
            SelectedEntity = HandleTraits::NullHandle;
            return;
        }

        ImGui::Separator();
        ImGui::Text("Tag     %s", entity->Tag.c_str());
        ImGui::Text("Handle  %d", HandleUtils::Index(handle));
        ImGui::Text("Version %d", HandleUtils::Version(handle));
        ImGui::Text("Layer   %d", HandleUtils::Layer(handle));
        ImGui::Text("Static  %s", HandleUtils::IsStatic(handle) ? "True" : "False");
        controls::Component(GetComponent<Transform>(handle));
        controls::Component(GetComponent<NetworkDispatcher>(handle));
        controls::Component(GetComponent<Renderer>(handle));
        #ifdef USE_VULKAN
        controls::Component(GetComponent<vulkan::MeshRenderer>(handle));
        controls::Component(GetComponent<vulkan::PointLight>(handle));
        #else
        controls::Component(GetComponent<ParticleEmitter>(handle));
        #endif
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

    void UtilitiesPanel(float* dtMult, ecs::registry_type* registry, unsigned drawCallCount, float windowWidth, float windowHeight)
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
                WrapTabItem("Systems", ComponentSystems, registry);
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

        for(auto v : {Filter::All, Filter::Logic, Filter::Physics, Filter::Rendering, Filter::User})
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

    template<class T>
    void ComponentSystemHeader(const char* name, std::span<T> components)
    {
        constexpr auto size = static_cast<unsigned>(sizeof(T));
        constexpr auto destruction = StoragePolicy<T>::allow_trivial_destruction::value ? "False" : "True";
        constexpr auto sorting = StoragePolicy<T>::sort_dense_storage_by_address::value ? "True" : "False";

        if(ImGui::CollapsingHeader(name))
        {
            ImGui::PushID(name);
            ImGui::Indent();
            ImGui::Text("Component Size:      %u", size);
            ImGui::Text("Copmonent Count:     %u", static_cast<unsigned>(components.size()));
            ImGui::Text("Require Destruction: %s", destruction);
            ImGui::Text("Sort by Address:     %s", sorting);
            if(ImGui::CollapsingHeader("Components"))
            {
                ImGui::Indent();
                for(const auto& component : components)
                    ImGui::Text("Handle: %5u  |  Address: %p", HandleUtils::Index(component.GetParentHandle()), static_cast<const void*>(&component));
                ImGui::Unindent();
            }
            ImGui::Unindent();
            ImGui::PopID();
        }
    }

    /** @todo this will eventually need to be generic */
    void ComponentSystems(ecs::registry_type* registry)
    {
        ComponentSystemHeader<Collider>("Collider", registry->ViewAll<Collider>());
        ComponentSystemHeader<NetworkDispatcher>("NetworkDispatcher", registry->ViewAll<NetworkDispatcher>());
        ComponentSystemHeader<ParticleEmitter>("Particle Emitter", registry->ViewAll<ParticleEmitter>());
        ComponentSystemHeader<PointLight>("Point Light", registry->ViewAll<PointLight>());
        ComponentSystemHeader<Renderer>("Renderer", registry->ViewAll<Renderer>());
        ComponentSystemHeader<Transform>("Transform", registry->ViewAll<Transform>());
        #ifdef USE_VULKAN
        ComponentSystemHeader<nc::vulkan::MeshRenderer>("Mesh Renderer", registry->ViewAll<nc::vulkan::MeshRenderer>());
        ComponentSystemHeader<nc::vulkan::PointLight>("Point Light", registry->ViewAll<nc::vulkan::PointLight>());
        #else
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