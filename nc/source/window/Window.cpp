#include "Window.h"
#include "DebugUtils.h"

namespace nc
{
    std::function<Vector2()> Window::GetDimensions_ = nullptr;
    std::function<void(window::IOnResizeReceiver*)> Window::RegisterOnResizeReceiver_ = nullptr;
    std::function<void(window::IOnResizeReceiver*)> Window::UnregisterOnResizeReceiver_ = nullptr;

    Vector2 Window::GetDimensions()
    {
        IF_THROW(!Window::GetDimensions_, "Window::GetDimensions_ is not bound");
        return Window::GetDimensions_();
    }

    void Window::RegisterOnResizeReceiver(window::IOnResizeReceiver* receiver)
    {
        IF_THROW(!Window::RegisterOnResizeReceiver_, "Window::RegisterOnResizeReceiver_ is not bound");
        Window::RegisterOnResizeReceiver_(receiver);
    }

    void Window::UnregisterOnResizeReceiver(window::IOnResizeReceiver* receiver)
    {
        IF_THROW(!Window::UnregisterOnResizeReceiver_, "Window::UnregisterOnResizeReceiver_ is not bound");
        Window::UnregisterOnResizeReceiver_(receiver);
    }

} // end namespace nc