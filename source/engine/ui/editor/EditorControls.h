#pragma once
#ifdef NC_EDITOR_ENABLED

#include "audio/AudioSource.h"
#include "ecs/Logic.h"
#include "ecs/Registry.h"
#include "ecs/Tag.h"
#include "ecs/Transform.h"
#include "ecs/View.h"
#include "ecs/detail/FreeComponentGroup.h"
#include "graphics/Camera.h"
#include "graphics/MeshRenderer.h"
#include "graphics/ParticleEmitter.h"
#include "graphics/PointLight.h"
#include "graphics/ToonRenderer.h"
#include "network/NetworkDispatcher.h"
#include "physics/Collider.h"
#include "physics/ConcaveCollider.h"
#include "physics/PhysicsBody.h"

#include "imgui/imgui.h"


namespace nc::ui::editor::controls
{
    auto SelectedEntity = Entity::Null();
    const auto TitleBarHeight = 40.0f;
    const auto DefaultItemWidth = 60.0f;
    const auto SceneGraphPanelWidth = 300;
    const auto UtilitiesPanelHeight = 350;
    const auto GraphSize = ImVec2{128, 32};
    const auto Padding = 4.0f;

    inline void SceneGraphPanel(Registry* registry, float windowHeight);
    inline void SceneGraphNode(Registry* registry, Entity entity, Tag* tag, Transform* transform);
    inline void EntityPanel(Registry* registry, Entity entity);
    inline void FreeComponentElement(FreeComponent* comp);
    inline void UtilitiesPanel(float* dtMult, Registry* registry, float windowWidth, float windowHeight);
    inline void FrameData(float* dtMult);
    inline void ComponentSystems(Registry* registry);
    inline void PhysicsMetrics();

    /**
     * Scene Graph Controls
     */
    void SceneGraphPanel(Registry* registry, float windowHeight)
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
                for(auto entity : View<Entity>{registry})
                {
                    auto* transform = registry->Get<Transform>(entity);
                    auto* tag = registry->Get<Tag>(entity);
                    if(transform->Parent().Valid()) // only draw root nodes
                        continue;

                    if(!filter.PassFilter(tag->Value().data()))
                        continue;
                    
                    SceneGraphNode(registry, entity, tag, transform);
                }
            } ImGui::EndChild();

            if(ImGui::BeginChild("EntityPanel", {0,0}, true))
            {
                if(SelectedEntity.Valid())
                    controls::EntityPanel(registry, SelectedEntity);

            } ImGui::EndChild();

        } ImGui::EndChild();
    }

    void SceneGraphNode(Registry* registry, Entity entity, Tag* tag, Transform* transform)
    {
        ImGui::PushID(entity.Index());

        auto flags = 0;
        if(SelectedEntity == entity)
            flags = flags | ImGuiTreeNodeFlags_Framed;

        auto open = ImGui::TreeNodeEx(tag->Value().data(), flags);
        if(ImGui::IsItemClicked())
            SelectedEntity = entity;
        
        if(open)
        {
            for(auto child : transform->Children())
                SceneGraphNode(registry, child, registry->Get<Tag>(child), registry->Get<Transform>(child));

            ImGui::TreePop();
        }

        ImGui::PopID();
    }

    template<class T>
    void ComponentElement(Registry* registry, Entity entity)
    {
        if (auto* component = registry->Get<T>(entity))
        {
            ImGui::Spacing(); ImGui::Separator(); ImGui::Spacing();
            ComponentGuiElement(component);
        }
    }

    void EntityPanel(Registry* registry, Entity entity)
    {
        if(!registry->Contains<Entity>(entity)) // entity may have been deleted
        {
            SelectedEntity = Entity::Null();
            return;
        }

        ImGui::Separator();
        ImGui::Text("Tag     %s", registry->Get<Tag>(entity)->Value().data());
        ImGui::Text("Index   %d", entity.Index());
        ImGui::Text("Layer   %d", entity.Layer());
        ImGui::Text("Static  %s", entity.IsStatic() ? "True" : "False");

        /** @todo #425 Only Transform has an AnyComponent interface. Update remaining components. */
        for (auto& any : registry->GetAllComponentsOn(entity))
        {
            any.DrawUI();
        }

        ComponentElement<graphics::MeshRenderer>(registry, entity);
        ComponentElement<graphics::PointLight>(registry, entity);
        ComponentElement<graphics::ToonRenderer>(registry, entity);
        ComponentElement<graphics::ParticleEmitter>(registry, entity);
        ComponentElement<physics::PhysicsBody>(registry, entity);
        if (auto* col = registry->Get<physics::Collider>(entity); col)
        {
            // collider model doesn't update/submit unless we tell it to
            col->SetEditorSelection(true);
            ImGui::Spacing(); ImGui::Separator(); ImGui::Spacing();
            ComponentGuiElement(col);
        }

        ComponentElement<physics::ConcaveCollider>(registry, entity);
        ComponentElement<net::NetworkDispatcher>(registry, entity);

        for(const auto& comp : registry->Get<ecs::detail::FreeComponentGroup>(entity)->GetComponents())
            controls::FreeComponentElement(comp);

        ImGui::Separator();
    }

    void FreeComponentElement(FreeComponent* comp)
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

    void UtilitiesPanel(float* dtMult, Registry* registry, float windowWidth, float windowHeight)
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
                ImGui::SetColumnWidth(-1, 0.85f * panelWidth);
                initColumnWidth = true;
            }
            if(ImGui::BeginTabBar("UtilitiesLeftTabBar"))
            {
                WrapTabItem("Systems", ComponentSystems, registry);
                ImGui::EndTabBar();
            }

            ImGui::NextColumn();
            if(ImGui::BeginTabBar("UtilitiesRightTabBar"))
            {
                WrapTabItem("Frame Data", FrameData, dtMult);
                WrapTabItem("UI Metrics", ImGui::ShowMetricsWindow, nullptr);
                ImGui::EndTabBar();
            }
        }
        ImGui::EndChild();
    }

    void FrameData(float* dtMult)
    {
        float frameRate = ImGui::GetIO().Framerate;
        ImGui::SetNextItemWidth(DefaultItemWidth);
        ImGui::DragFloat("dtX", dtMult, 0.1f, 0.0f, 5.0f, "%.1f");
        ImGui::Text("%.1f fps", frameRate);
        ImGui::Text("%.1f ms/frame", 1000.0f / frameRate);
    }

    template<class T>
    void ComponentSystemHeader(const char* name, View<T> components)
    {
        constexpr auto size = static_cast<unsigned>(sizeof(T));

        if(ImGui::CollapsingHeader(name))
        {
            ImGui::PushID(name);
            ImGui::Indent();
            ImGui::Text("Component Size:      %u", size);
            ImGui::Text("Copmonent Count:     %u", static_cast<unsigned>(components.size()));
            if(ImGui::CollapsingHeader("Components"))
            {
                ImGui::Indent();
                for(const auto& component : components)
                    ImGui::Text("Handle: %5u  |  Address: %p", component.ParentEntity().Index(), static_cast<const void*>(&component));
                ImGui::Unindent();
            }
            ImGui::Unindent();
            ImGui::PopID();
        }
    }

    /** @todo this will eventually need to be generic */
    void ComponentSystems(Registry* registry)
    {
        ComponentSystemHeader<physics::Collider>("Collider", View<physics::Collider>(registry));
        ComponentSystemHeader<net::NetworkDispatcher>("NetworkDispatcher", View<net::NetworkDispatcher>(registry));
        ComponentSystemHeader<graphics::ParticleEmitter>("Particle Emitter", View<graphics::ParticleEmitter>(registry));
        ComponentSystemHeader<physics::PhysicsBody>("Physics Body", View<physics::PhysicsBody>(registry));
        ComponentSystemHeader<Transform>("Transform", View<Transform>(registry));
        ComponentSystemHeader<graphics::MeshRenderer>("Mesh Renderer", View<graphics::MeshRenderer>(registry));
        ComponentSystemHeader<graphics::PointLight>("Point Light", View<graphics::PointLight>(registry));
        ComponentSystemHeader<graphics::ToonRenderer>("Toon Renderer", View<graphics::ToonRenderer>(registry));
    }

} // end namespace nc::ui::editor
#endif
