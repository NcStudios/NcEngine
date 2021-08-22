#pragma once
#ifdef NC_EDITOR_ENABLED
#include "debug/Profiler.h"
#include "Ecs.h"
#include "ecs/EntityComponentSystem.h"
#include "graphics/d3dresource/GraphicsResourceManager.h"
#include "graphics/vulkan/resources/ResourceManager.h"
#include "imgui/imgui.h"

namespace nc::ui::editor::controls
{
    auto SelectedEntity = EntityTraits::NullHandle;
    const auto TitleBarHeight = 40.0f;
    const auto DefaultItemWidth = 60.0f;
    const auto SceneGraphPanelWidth = 300;
    const auto UtilitiesPanelHeight = 350;
    const auto GraphSize = ImVec2{128, 32};
    const auto Padding = 4.0f;

    inline void SceneGraphPanel(registry_type* registry, float windowHeight);
    inline void SceneGraphNode(registry_type* registry, Entity entity, Tag* tag, Transform* transform);
    inline void EntityPanel(registry_type* registry, Entity entity);
    inline void AutoComponentElement(AutoComponent* comp);
    inline void UtilitiesPanel(float* dtMult, registry_type* registry, unsigned drawCallCount, float windowWidth, float windowHeight);
    inline void GraphicsResourcePanel();
    inline void FrameData(float* dtMult, unsigned drawCallCount);
    inline void Profiler();
    inline void ComponentSystems(registry_type* registry);
    inline void PhysicsMetrics();

    /**
     * Scene Graph Controls
     */
    void SceneGraphPanel(registry_type* registry, float windowHeight)
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
                for(auto entity : registry->ViewAll<Entity>())
                {
                    auto* transform = registry->Get<Transform>(entity);
                    auto* tag = registry->Get<Tag>(entity);
                    if(transform->GetParent().Valid()) // only draw root nodes
                        continue;

                    if(!filter.PassFilter(tag->Value().data()))
                        continue;
                    
                    SceneGraphNode(registry, entity, tag, transform);
                }
            } ImGui::EndChild();

            if(ImGui::BeginChild("EntityPanel", {0,0}, true))
            {
                if(SelectedEntity != EntityTraits::NullHandle)
                    controls::EntityPanel(registry, static_cast<Entity>(SelectedEntity));

            } ImGui::EndChild();

        } ImGui::EndChild();
    }

    void SceneGraphNode(registry_type* registry, Entity entity, Tag* tag, Transform* transform)
    {
        auto handleValue = static_cast<EntityTraits::underlying_type>(entity);
        ImGui::PushID(handleValue);

        auto flags = 0;
        if(SelectedEntity == handleValue)
            flags = flags | ImGuiTreeNodeFlags_Framed;

        auto open = ImGui::TreeNodeEx(tag->Value().data(), flags);
        if(ImGui::IsItemClicked())
            SelectedEntity = handleValue;
        
        if(open)
        {
            for(auto child : transform->GetChildren())
                SceneGraphNode(registry, child, registry->Get<Tag>(child), registry->Get<Transform>(child));

            ImGui::TreePop();
        }

        ImGui::PopID();
    }

    void EntityPanel(registry_type* registry, Entity entity)
    {
        if(!registry->Contains<Entity>(entity)) // entity may have been deleted
        {
            SelectedEntity = EntityTraits::NullHandle;
            return;
        }

        ImGui::Separator();
        ImGui::Text("Tag     %s", registry->Get<Tag>(entity)->Value().data());
        ImGui::Text("Index   %d", EntityUtils::Index(entity));
        ImGui::Text("Version %d", EntityUtils::Version(entity));
        ImGui::Text("Layer   %d", EntityUtils::Layer(entity));
        ImGui::Text("Static  %s", EntityUtils::IsStatic(entity) ? "True" : "False");

        if(auto* transform = registry->Get<Transform>(entity); transform)
        {
            ImGui::Spacing(); ImGui::Separator(); ImGui::Spacing();
            ComponentGuiElement(transform);
        }

        if(auto* renderer = registry->Get<Renderer>(entity))
        {
            ImGui::Spacing(); ImGui::Separator(); ImGui::Spacing();
            ComponentGuiElement(renderer);
        }

        #ifdef USE_VULKAN
        if(auto* meshRenderer = registry->Get<vulkan::MeshRenderer>(entity))
            ComponentGuiElement(meshRenderer);
        if (auto* pointLight = registry->Get<vulkan::PointLight>(entity))
            ComponentGuiElement(pointLight);
        #endif

        if(auto* body = registry->Get<PhysicsBody>(entity))
        {
            ImGui::Spacing(); ImGui::Separator(); ImGui::Spacing();
            ComponentGuiElement(body);
        }

        if(auto* emitter = registry->Get<ParticleEmitter>(entity))
        {
            ImGui::Spacing(); ImGui::Separator(); ImGui::Spacing();
            ComponentGuiElement(emitter);
        
        }

        if(auto* dispatcher = registry->Get<NetworkDispatcher>(entity))
        {
            ImGui::Spacing(); ImGui::Separator(); ImGui::Spacing();
            ComponentGuiElement(dispatcher);
        }

        if(auto* light = registry->Get<PointLight>(entity))
        {
            ImGui::Spacing(); ImGui::Separator(); ImGui::Spacing();
            ComponentGuiElement(light);
        }

        if(auto* col = registry->Get<Collider>(entity); col)
        {
            // collider model doesn't update/submit unless we tell it to
            col->SetEditorSelection(true);
            ImGui::Spacing(); ImGui::Separator(); ImGui::Spacing();
            ComponentGuiElement(col);
        }

        for(const auto& comp : registry->Get<AutoComponentGroup>(entity)->GetAutoComponents())
            controls::AutoComponentElement(comp);

        ImGui::Separator();
    }

    void AutoComponentElement(AutoComponent* comp)
    {
        if(!comp)
            return;
        ImGui::Spacing(); ImGui::Separator(); ImGui::Spacing();
        ImGui::BeginGroup();
            comp->ComponentGuiElement();
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

    void UtilitiesPanel(float* dtMult, registry_type* registry, unsigned drawCallCount, float windowWidth, float windowHeight)
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

        for(auto v : {Filter::All, Filter::Logic, Filter::Collision, Filter::Dynamics, Filter::Rendering, Filter::User})
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
                    ImGui::Text("Handle: %5u  |  Address: %p", EntityUtils::Index(component.GetParentEntity()), static_cast<const void*>(&component));
                ImGui::Unindent();
            }
            ImGui::Unindent();
            ImGui::PopID();
        }
    }

    /** @todo this will eventually need to be generic */
    void ComponentSystems(registry_type* registry)
    {
        ComponentSystemHeader<Collider>("Collider", registry->ViewAll<Collider>());
        ComponentSystemHeader<NetworkDispatcher>("NetworkDispatcher", registry->ViewAll<NetworkDispatcher>());
        ComponentSystemHeader<ParticleEmitter>("Particle Emitter", registry->ViewAll<ParticleEmitter>());
        ComponentSystemHeader<PhysicsBody>("Physics Body", registry->ViewAll<PhysicsBody>());
        ComponentSystemHeader<PointLight>("Point Light", registry->ViewAll<PointLight>());
        ComponentSystemHeader<Renderer>("Renderer", registry->ViewAll<Renderer>());
        ComponentSystemHeader<Transform>("Transform", registry->ViewAll<Transform>());
        #ifdef USE_VULKAN
        ComponentSystemHeader<nc::vulkan::MeshRenderer>("Mesh Renderer", registry->ViewAll<nc::vulkan::MeshRenderer>());
        ComponentSystemHeader<nc::vulkan::PointLight>("Point Light", registry->ViewAll<nc::vulkan::PointLight>());
        #endif
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