#include "Window.h"

namespace nc
{
    std::function<Vector2()> Window::GetDimensions_ = nullptr;

    Vector2 Window::GetDimensions()
    {
        return Window::GetDimensions_();
    }
} // end namespace nc