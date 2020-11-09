#include "NcWindow.h"
#include "window/Window.h"

namespace nc
{
    Vector2 NcGetWindowDimensions()
    {
        return Window::Instance->GetWindowDimensions();
    }
}