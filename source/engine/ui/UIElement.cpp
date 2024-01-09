#include "ui/UIElement.h"
#include "window/Window.h"
#include "imgui/imgui.h"

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

    UIFixedElement::UIFixedElement(bool startOpen, UIPosition position, Vector2 dimensions)
        : UIElement(startOpen),
          m_position{ position },
          m_screenDimensions { window::GetScreenDimensions() },
          m_elementDimensions{ dimensions }
    {
        CalculateTopLeftPosition();
        window::RegisterOnResizeReceiver(this);
    }

    UIFixedElement::~UIFixedElement() noexcept
    {
        window::UnregisterOnResizeReceiver(this);
    }

    void UIFixedElement::OnResize(Vector2 dimensions)
    {
        m_screenDimensions = dimensions;
        CalculateTopLeftPosition();
    }

    void UIFixedElement::PositionElement()
    {
        ImGui::SetNextWindowPos({m_topLeftPosition.x, m_topLeftPosition.y});
        ImGui::SetNextWindowSize({m_elementDimensions.x, m_elementDimensions.y});
    }

    void UIFixedElement::CalculateTopLeftPosition()
    {
        m_topLeftPosition = utils::GetTopLeftCoords( m_position, m_screenDimensions, m_elementDimensions);
    }
}