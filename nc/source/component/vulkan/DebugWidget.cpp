#include "component/vulkan/DebugWidget.h"
#include "Ecs.h"

#ifdef NC_EDITOR_ENABLED
#include "ui/editor/Widgets.h"
#endif

namespace nc::vulkan
{
    DebugWidget::DebugWidget(Entity entity, WidgetShape shape)
        : ComponentBase{entity}
    {
        switch(shape)
        {
            case WidgetShape::Cube:
            {
                m_meshUid = CubeMeshPath;
                break;
            }
            case WidgetShape::Sphere:
            {
                m_meshUid = SphereMeshPath;
                break;
            }
            case WidgetShape::Capsule:
            {
                m_meshUid = CapsuleMeshPath;
                break;
            }
            case WidgetShape::Hull:
            {
                m_meshUid = SphereMeshPath;
                break;
            }
            default:
            {
                m_meshUid = SphereMeshPath;
                break;
            }
        }

    }

    bool DebugWidget::IsEnabled() const noexcept
    {
        return m_isEnabled;
    }

    void DebugWidget::Enable(bool isEnabled) noexcept
    {
        m_isEnabled = isEnabled;
    }

    Transform* DebugWidget::GetTransform()
    {
        return ActiveRegistry()->Get<Transform>(GetParentEntity());
    }

    DirectX::FXMMATRIX DebugWidget::GetTransformationMatrix() const noexcept
    {
        return m_transformationMatrix;
    }

    void DebugWidget::SetTransformationMatrix(DirectX::FXMMATRIX matrix) noexcept
    {
        m_transformationMatrix = matrix;
    }

    const std::string& DebugWidget::GetMeshUid() const
    {
        return m_meshUid;
    }
}

namespace nc
{
    #ifdef NC_EDITOR_ENABLED
    template<> void ComponentGuiElement<vulkan::DebugWidget>(vulkan::DebugWidget* debugWidget)
    {
        ImGui::Text("Debug widget");
    }
    #endif
}