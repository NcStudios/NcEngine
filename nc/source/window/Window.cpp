#include "Window.h"
#include "WindowImpl.h"

namespace nc
{
    window::WindowImpl* Window::m_impl = nullptr;

    void Window::RegisterImpl(window::WindowImpl* impl)
    {
        Window::m_impl = impl;
    }

    Vector2 Window::GetDimensions()
    {
        return Window::m_impl->GetDimensions();
    }
} // end namespace nc