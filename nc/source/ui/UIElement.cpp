#include "ui/UIElement.h"
#include "Window.h"

namespace nc::ui
{
    UIElement::UIElement(bool startOpen)
        : isOpen(startOpen)
    {
    }

    void UIElement::ToggleOpen()
    {
        isOpen = !isOpen;
    }

    UIFixedElement::UIFixedElement(bool startOpen, UIPosition position, ImVec2 dimensions)
        : UIElement(startOpen),
          m_position{ position },
          m_screenDimensions { window::GetDimensions().ToImVec2() },
          m_elementDimensions{ dimensions }
    {
        CalculateTopLeftPosition();
        window::RegisterOnResizeReceiver(this);
    }

    UIFixedElement::~UIFixedElement()
    {
        window::UnregisterOnResizeReceiver(this);
    }

    void UIFixedElement::OnResize(Vector2 dimensions)
    {
        m_screenDimensions = dimensions.ToImVec2();
        CalculateTopLeftPosition();
    }

    void UIFixedElement::PositionElement()
    {
        ImGui::SetNextWindowPos(m_topLeftPosition);
        ImGui::SetNextWindowSize(m_elementDimensions);
    }

    void UIFixedElement::CalculateTopLeftPosition()
    {
        m_topLeftPosition = utils::GetTopLeftCoords( m_position, m_screenDimensions, m_elementDimensions);
    }
}