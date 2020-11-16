#include "IUIElement.h"
#include "Window.h"

namespace nc::ui
{
    IUIFixedElement::IUIFixedElement(bool startOpen, UIPosition position, ImVec2 dimensions)
        : IUIElement(startOpen),
          m_position{ position },
          m_screenDimensions { Window::GetDimensions() },
          m_elementDimensions{ dimensions }
    {
        CalculateTopLeftPosition();
        Window::RegisterOnResizeReceiver(this);
    }

    IUIFixedElement::~IUIFixedElement()
    {
        Window::UnregisterOnResizeReceiver(this);
    }

    void IUIFixedElement::OnResize(Vector2 dimensions)
    {
        m_screenDimensions = {dimensions.X(), dimensions.Y()};
        CalculateTopLeftPosition();
    }

    void IUIFixedElement::PositionElement()
    {
        ImGui::SetNextWindowPos(m_topLeftPosition);
        ImGui::SetNextWindowSize(m_elementDimensions);
    }

    void IUIFixedElement::CalculateTopLeftPosition()
    {
        m_topLeftPosition = utils::GetTopLeftCoords( m_position, {m_screenDimensions.X(), m_screenDimensions.Y()}, m_elementDimensions);
    }
}