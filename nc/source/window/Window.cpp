#include "Window.h"
#include "DebugUtils.h"

namespace nc
{
    std::function<Vector2()> Window::GetDimensions_ = nullptr;

    Vector2 Window::GetDimensions()
    {
        IF_THROW(!Window::GetDimensions_, "Window::GetDimensions_ is not bound");
        return Window::GetDimensions_();
    }
} // end namespace nc