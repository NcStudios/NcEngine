#include "SceneGraph.h"
#include "EntityContextMenu.h"
#include "ncengine/ecs/Tag.h"
#include "ncengine/ecs/Transform.h"
#include "ncengine/ecs/View.h"
#include "ncengine/ui/ImGuiUtility.h"

namespace
{
auto IsRoot(nc::Transform* transform) -> bool
{
    return !transform->Parent().Valid();
}
} // anonymous namespace

namespace nc::ui::editor
{
void SceneGraph::Draw(ecs::Ecs world)
{
    EnsureSelection(world);
    m_tagFilter.Draw("search");

    ChildWindow("Entity List", [&]()
    {
        if (IsWindowBackgroundClicked())
        {
            m_selectedEntity = Entity::Null();
        }

        GraphContextMenu(world);
        Graph(world);
    });

    if (m_entityCreateWindowOpen)
    {
        ImGui::OpenPopup("Create Entity");
        ImGui::SetNextWindowSize(ImVec2{400.0f, 200.0f}, ImGuiCond_Once);
        if (ImGui::BeginPopupModal("Create Entity", &m_entityCreateWindowOpen))
        {
            static std::string tag = "Entity";
            static uint8_t layer = 0;
            static bool staticFlag = false;
            static bool persistentFlag = false;
            static bool noCollisionFlag = false;
            static bool noSerializeFlag = false;

            ui::InputText(tag, "tag");
            ui::InputU8(layer, "layer");
            ImGui::Checkbox("static", &staticFlag);
            ImGui::Checkbox("persistent", &persistentFlag);
            ImGui::Checkbox("noCollisionNotifications", &noCollisionFlag);
            ImGui::Checkbox("noSerialize", &noSerializeFlag);

            if (ImGui::Button("Create"))
            {
                auto flags = Entity::Flags::None;
                if (staticFlag) flags |= Entity::Flags::Static;
                if (persistentFlag) flags |= Entity::Flags::Persistent;
                if (noCollisionFlag) flags |= Entity::Flags::NoCollisionNotifications;
                if (noSerializeFlag) flags |= Entity::Flags::NoSerialize;
                world.Emplace<Entity>({.tag = tag, .layer = layer, .flags = flags});
                m_entityCreateWindowOpen = false;
            }
            ImGui::EndPopup();
        }
    }
}

void SceneGraph::EnsureSelection(ecs::Ecs world)
{
    if (m_selectedEntity.Valid() && !world.Contains<Entity>(m_selectedEntity))
    {
        m_selectedEntity = Entity::Null();
    }
}

auto SceneGraph::PassFilter(Tag& tag) -> bool
{
    return m_tagFilter.PassFilter(tag.Value().data());
}

void SceneGraph::Graph(ecs::Ecs world)
{
    for(auto entity : world.GetAll<Entity>())
    {
        auto transform = world.Get<Transform>(entity);
        auto tag = world.Get<Tag>(entity);
        if (IsRoot(transform) && PassFilter(*tag))
        {
            GraphNode(world, entity, *tag, *transform);
        }
    }
}

void SceneGraph::GraphNode(ecs::Ecs world, Entity entity, Tag& tag, Transform& transform)
{
    IMGUI_SCOPE(ImGuiId, entity.Index());
    const auto flags = m_selectedEntity == entity 
        ? ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_OpenOnArrow
        : 0;

    const auto isNodeExpanded = ImGui::TreeNodeEx(tag.Value().data(), flags);

    if (ImGui::IsItemClicked())
    {
        m_selectedEntity = entity;
    }
    else if (ImGui::BeginPopupContextItem())
    {
        m_selectedEntity = EntityContextMenu(entity, world);
        ImGui::EndPopup();
    }

    DragAndDropSource<Entity>(&entity);
    DragAndDropTarget<Entity>([entity, world](Entity* source) mutable
    {
        world.Get<Transform>(*source)->SetParent(entity);
    });

    if(isNodeExpanded)
    {
        for(auto child : transform.Children())
        {
            GraphNode(world, child, *world.Get<Tag>(child), *world.Get<Transform>(child));
        }

        ImGui::TreePop();
    }
}

void SceneGraph::GraphContextMenu(ecs::Ecs world)
{
    if (ImGui::BeginPopupContextWindow())
    {
        if (ImGui::Selectable("New Default Entity"))
            m_selectedEntity = world.Emplace<Entity>(EntityInfo{});
        // else if (ImGui::Selectable("New Static Entity"))
            // m_selectedEntity = world.Emplace<Entity>(EntityInfo{.flags = Entity::Flags::Static});
        else if (ImGui::Selectable("New Entity"))
        {
            m_entityCreateWindowOpen = true;
        }
        else
            m_selectedEntity = Entity::Null();

        ImGui::EndPopup();
    }
}
} // namespace nc::ui::editor
