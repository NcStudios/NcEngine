#include "component/Collider.h"
#include "Ecs.h"
#include "graphics/d3dresource/ConstantBufferResources.h"

#ifdef NC_EDITOR_ENABLED
#include "ui/editor/Widgets.h"
#include <charconv>
#endif

namespace
{
    const auto CubeMeshPath = std::string{"project/Models/cube.fbx"};
    auto CreateMaterial = nc::graphics::Material::CreateMaterial<nc::graphics::TechniqueType::Wireframe>;

    auto CreateBoxModel()
    {
        return nc::graphics::Model{ {CubeMeshPath}, CreateMaterial() };
    }
}

namespace nc
{
    Collider::Collider(EntityHandle handle, Vector3 scale)
        : ComponentBase(handle),
          m_transform{GetComponent<Transform>(handle)},
          m_scale{scale},
          m_model{CreateBoxModel()},
          m_currentCollisions{}
    {}

    Collider::~Collider() = default;

    void Collider::Update(graphics::FrameManager& frame)
    {
        IF_THROW(!m_transform, "ColliderBase::Update - Bad Transform Ptr");
        m_model.SetTransformationMatrix(m_transform->GetTransformationMatrixEx(m_scale));
        m_model.Submit(frame);
    }

    DirectX::FXMMATRIX Collider::GetTransformationMatrix() const
    {
        return m_model.GetTransformationMatrix();
    }

    void Collider::UpdateCollisions(std::vector<Collider*> collisions)
    {
        auto entity = GetEntity(GetParentHandle());

        for(auto prevCollision : collisions)
        {
            auto pos = std::find(collisions.begin(), collisions.end(), prevCollision);
            auto otherEntity = GetEntity(prevCollision->GetParentHandle());

            if(pos == collisions.end())
            {
                entity->SendOnCollisionExit(otherEntity);
                otherEntity->SendOnCollisionExit(entity);
            }
            else
            {
                entity->SendOnCollisionStay(otherEntity);
                otherEntity->SendOnCollisionStay(entity);
            }
        }

        for(auto newCollision : collisions)
        {
            auto pos = std::find(m_currentCollisions.begin(), m_currentCollisions.end(), newCollision);
            if(pos == m_currentCollisions.end())
            {
                auto otherEntity = GetEntity(newCollision->GetParentHandle());
                entity->SendOnCollisionEnter(otherEntity);
                otherEntity->SendOnCollisionEnter(entity);
            }
        }

        m_currentCollisions = std::move(collisions);
    }

    #ifdef NC_EDITOR_ENABLED
    void Collider::EditorGuiElement()
    {
        ImGui::Text("Collider");
        ui::editor::xyzWidgetHeader("     ");
        ui::editor::xyzWidget("Scale", "colliderscale", &m_scale.x, &m_scale.y, &m_scale.z, 0.01f, 100.0f);
        if(ImGui::CollapsingHeader("Current Collisions"))
        {
            std::array<char, 8> buf;

            for(auto c : m_currentCollisions)
            {
                std::to_chars(buf.data(), buf.data() + buf.size(), static_cast<unsigned>(c->GetParentHandle()));
                ImGui::Indent();
                ImGui::Text("EntityHandle");
                ImGui::SameLine();
                ImGui::Text(buf.data());
                ImGui::Unindent();
            }
        }
    }
    #endif
}