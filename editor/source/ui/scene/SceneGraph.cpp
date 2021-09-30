#include "SceneGraph.h"
#include "Window.h"
#include "utility/DefaultComponents.h"
#include "utility/Output.h"

#include <iostream>

namespace
{
    using namespace nc;

    const auto TitleBarHeight = 40.0f;
    const auto DefaultItemWidth = 60.0f;
    const auto SceneGraphPanelWidth = 300;
    const auto Padding = 4.0f;
}

namespace nc::editor
{
    SceneGraph::SceneGraph(registry_type* registry, SceneCallbacks sceneCallbacks, EntityCallbacks::ChangeTagCallbackType changeTagCallback, std::string projectName)
        : m_registry{registry},
          m_projectName{std::move(projectName)},
          m_inspector{registry},
          m_sceneSelectWidget{std::move(sceneCallbacks)},
          m_changeTagCallback{std::move(changeTagCallback)},
          m_dimensions{window::GetDimensions()},
          m_selectedEntity{EntityTraits::NullHandle}
    {
        window::RegisterOnResizeReceiver(this);
    }

    SceneGraph::~SceneGraph()
    {
        window::UnregisterOnResizeReceiver(this);
    }

    void SceneGraph::SetProjectName(std::string name)
    {
        m_projectName = std::move(name);
    }

    void SceneGraph::UpdateScenes(std::vector<std::string> scenes, int selectedScene)
    {
        m_sceneSelectWidget.UpdateScenes(std::move(scenes), selectedScene);
    }

    void SceneGraph::OnResize(Vector2 dimensions)
    {
        m_dimensions = dimensions;
    }

    void SceneGraph::Draw()
    {
        ImGui::SetNextWindowPos({Padding, TitleBarHeight});
        auto sceneGraphHeight = m_dimensions.y - TitleBarHeight;

        if(ImGui::BeginChild("ScenePanel", {SceneGraphPanelWidth, sceneGraphHeight}, true))
        {
            if(m_projectName.empty())
            {
                ImGui::Text("No Project Open");
                ImGui::EndChild();
                return;
            }

            ImGui::Text("Project: %s", m_projectName.c_str());

            m_sceneSelectWidget.Draw();

            ImGui::Separator();
            ImGui::Separator();
            ImGui::Spacing();

            if(ImGui::BeginChild("EntityList", {0, sceneGraphHeight / 2}, true))
            {
                if(ImGui::BeginPopupContextWindow())
                {
                    SceneGraphContextMenu();
                    ImGui::EndPopup();
                }

                for(auto entity : m_registry->ViewAll<Entity>())
                {
                    auto* transform = m_registry->Get<Transform>(entity);
                    if(transform->GetParent().Valid()) // only draw root nodes
                        continue;

                    auto* tag = m_registry->Get<Tag>(entity);
                    SceneGraphNode(entity, tag, transform);
                }
            } ImGui::EndChild();

            if(ImGui::BeginChild("EntityPanel", {0,0}, true))
            {
                if(m_selectedEntity != EntityTraits::NullHandle)
                    EntityPanel(static_cast<Entity>(m_selectedEntity));

            } ImGui::EndChild();

        } ImGui::EndChild();
    }

    void SceneGraph::SceneGraphNode(Entity entity, Tag* tag, Transform* transform)
    {
        auto handleValue = static_cast<EntityTraits::underlying_type>(entity);
        ImGui::PushID(handleValue);

        auto flags = 0;
        if(m_selectedEntity == handleValue)
            flags = flags | ImGuiTreeNodeFlags_Framed;

        auto open = ImGui::TreeNodeEx(tag->Value().data(), flags);
        if(ImGui::IsItemClicked())
            m_selectedEntity = handleValue;
        
        if(ImGui::BeginPopupContextItem())
        {
            m_selectedEntity = handleValue;
            EntityContextMenu(entity);
            ImGui::EndPopup();
        }

        if(open)
        {
            for(auto child : transform->GetChildren())
                SceneGraphNode(child, m_registry->Get<Tag>(child), m_registry->Get<Transform>(child));

            ImGui::TreePop();
        }

        ImGui::PopID();
    }

    void SceneGraph::EntityPanel(Entity entity)
    {
        if(!m_registry->Contains<Entity>(entity)) // entity may have been deleted
        {
            m_selectedEntity = EntityTraits::NullHandle;
            return;
        }

        m_inspector.InspectEntity(entity);
    }

    void SceneGraph::AutoComponentElement(AutoComponent* comp)
    {
        if(!comp)
            return;
        ImGui::Spacing(); ImGui::Separator(); ImGui::Spacing();
        ImGui::BeginGroup();
            comp->ComponentGuiElement();
        ImGui::EndGroup();
    }

    void SceneGraph::SceneGraphContextMenu()
    {
        if(ImGui::Selectable("New Entity"))
        {
            auto entity = m_registry->Add<Entity>(EntityInfo{});
            m_selectedEntity = static_cast<EntityTraits::underlying_type>(entity);
        }
        else if(ImGui::Selectable("New Static Entity"))
        {
            auto entity = m_registry->Add<Entity>(EntityInfo{.flags = EntityFlags::Static});
            m_selectedEntity = static_cast<EntityTraits::underlying_type>(entity);
        }
        else if(ImGui::Selectable("New PointLight"))
        {
            auto entity = m_registry->Add<Entity>(EntityInfo{.tag = "PointLight"});
            m_registry->Add<PointLight>(entity, PointLightInfo{});
            m_selectedEntity = static_cast<EntityTraits::underlying_type>(entity);
        }
        else
        {
            m_selectedEntity = EntityTraits::NullHandle;
        }
    }

    void SceneGraph::EntityContextMenu(Entity entity)
    {
        bool hasCollider = m_registry->Contains<Collider>(entity);
        bool hasMeshRenderer = m_registry->Contains<MeshRenderer>(entity);
        bool hasPhysicsBody = m_registry->Contains<PhysicsBody>(entity);
        bool hasPointLight = m_registry->Contains<PointLight>(entity);

        if(ImGui::Selectable("Edit Tag"))
        {
            m_changeTagCallback(entity);
        }

        if(ImGui::Selectable("Add Child"))
        {
            m_registry->Add<Entity>(EntityInfo{.parent = entity});
        }

        if(ImGui::BeginMenu("Add Component"))
        {
            if(!hasCollider && ImGui::BeginMenu("Collider"))
            {
                if(ImGui::Selectable("Box")) m_registry->Add<Collider>(entity, BoxProperties{}, false);
                if(ImGui::Selectable("Capsule")) m_registry->Add<Collider>(entity, CapsuleProperties{}, false);
                if(ImGui::Selectable("Hull")) {}
                if(ImGui::Selectable("Sphere")) m_registry->Add<Collider>(entity, SphereProperties{}, false);
                ImGui::EndMenu();
            }

            if(!hasMeshRenderer && ImGui::Selectable("MeshRenderer"))
            {
                AddDefaultMeshRenderer(m_registry, entity);
            }

            if(!hasPhysicsBody && ImGui::Selectable("PhysicsBody"))
            {
                if(!hasCollider)
                    m_registry->Add<Collider>(entity, BoxProperties{}, false);

                m_registry->Add<PhysicsBody>(entity, PhysicsProperties{});
            }
            
            if(!hasPointLight && ImGui::Selectable("PointLight"))
            {
                m_registry->Add<PointLight>(entity, PointLightInfo{});
            }

            ImGui::EndMenu();
        }

        if(ImGui::BeginMenu("Remove Component"))
        {
            if(hasCollider && ImGui::Selectable("Collider"))
            {
                m_registry->Remove<Collider>(entity);
                if(hasPhysicsBody)
                    m_registry->Remove<PhysicsBody>(entity);
            }

            if(hasMeshRenderer && ImGui::Selectable("MeshRenderer"))
            {
                m_registry->Remove<MeshRenderer>(entity);
            }

            if(hasPhysicsBody && ImGui::Selectable("PhysicsBody"))
            {
                m_registry->Remove<PhysicsBody>(entity);
            }
            
            if(hasPointLight && ImGui::Selectable("PointLight"))
            {
                m_registry->Remove<PointLight>(entity);
            }

            ImGui::EndMenu();
        }

        if(ImGui::Selectable("Delete"))
        {
            m_registry->Remove<Entity>(entity);
        }
    }
}