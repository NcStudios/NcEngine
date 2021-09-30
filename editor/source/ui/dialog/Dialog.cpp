#include "Dialog.h"
#include "Window.h"

namespace
{
    ImVec2 ToImVec2(nc::Vector2 vec)
    {
        return ImVec2{vec.x, vec.y};
    }

    ImVec2 FindCenter(nc::Vector2 vec)
    {
        return ImVec2{vec.x * 0.5f, vec.y * 0.5f};
    }
}

namespace nc::editor
{
    DialogFixedCentered::DialogFixedCentered(Vector2 size)
        : m_position{FindCenter(window::GetDimensions())},
          m_size{ToImVec2(size)}
    {
        window::RegisterOnResizeReceiver(this);
    }

    DialogFixedCentered::~DialogFixedCentered()
    {
        window::UnregisterOnResizeReceiver(this);
    }

    void DialogFixedCentered::OnResize(Vector2 dimensions)
    {
        m_position = FindCenter(dimensions);
    }
}