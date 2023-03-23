#include "input/Input.h"
#include "InputInternal.h"

#include <algorithm>
#include <vector>
#include <windowsx.h>
#include "GLFW/glfw3.h"

#include <unordered_map>
#include <iostream>

namespace
{
enum class KeyState
{
    None,
    Pressed,
    Held,
    Released
};

auto ToKeyState(int action)
{
    switch(action)
    {
        case GLFW_PRESS: return KeyState::Pressed;
        case GLFW_RELEASE: return KeyState::Released;
        default: return KeyState::None;
    }
}
}

namespace nc::input
{
    struct
    {
        std::unordered_map<KeyCode, KeyState> keyStates = {};
        uint32_t mouseX = 0u;
        uint32_t mouseY = 0u;
        int32_t mouseWheel = 0;
        GLFWwindow* window = nullptr;
    } g_state;

    uint32_t MouseX() { return g_state.mouseX; }
    uint32_t MouseY() { return g_state.mouseY; }
    Vector2 MousePos() { return Vector2{(float)g_state.mouseX, (float)g_state.mouseY}; }
    int32_t MouseWheel() { return g_state.mouseWheel; }

    float GetXAxis() //eventually should bind inputs to axis from config file (controller/WASD/arrows)
    {
        float axis = 0.0f;
        if (KeyHeld(KeyCode::D)) { axis += 1.0; }
        if (KeyHeld(KeyCode::A)) { axis -= 1.0; }
        return axis;
    }

    float GetYAxis() //eventually binds inputs to axis from config file (controller/WASD/arrows)
    {
        float axis = 0.0f;
        if (KeyHeld(KeyCode::W)) { axis += 1.0; }
        if (KeyHeld(KeyCode::S)) { axis -= 1.0; }
        return axis;
    }

    Vector2 GetAxis()
    {
        return Vector2(GetXAxis(), GetYAxis());
    }

    bool KeyDown(KeyCode keyCode)
    {
        return g_state.keyStates.contains(keyCode) && g_state.keyStates[keyCode] == KeyState::Pressed;
    }

    bool KeyUp(KeyCode keyCode)
    {
        return g_state.keyStates.contains(keyCode) && g_state.keyStates[keyCode] == KeyState::Released;
    }

    bool KeyHeld(KeyCode keyCode)
    {
        return g_state.keyStates.contains(keyCode) && g_state.keyStates[keyCode] == KeyState::Held;
    }

    void SetMouseWheel(int xOffset, int yOffset)
    {
        g_state.mouseWheel = yOffset * 50;
    }

    void ResetMouseState()
    {
        g_state.mouseWheel = 0;
    }

    void AddKeyToQueue(KeyCode_t keyCode, int action)
    {
        g_state.keyStates[static_cast<KeyCode>(keyCode)] = ::ToKeyState(action);
    }

    void Flush()
    {
        // for (auto cur = beg; cur != end; )
        // {
        //     if (cur->second == KeyState::Pressed)
        //     {
        //         cur->second = KeyState::Held;
        //         ++cur;
        //     }
        //     else if (cur->second == KeyState::Released)
        //     {
        //         cur = g_state.keyStates.erase(cur);
        //     }
        // }

        for (auto& [key, state] : g_state.keyStates)
        {
            if (state == KeyState::Pressed || state == KeyState::None)
            {
                state = KeyState::Held;
            }
        }

        std::erase_if(g_state.keyStates, [](const auto& item)
        {
            return item.second == KeyState::None || item.second == KeyState::Released;
        });

        ResetMouseState();
    }

    void UpdateMousePosition(int mouseX, int mouseY)
    {
        g_state.mouseX = mouseX;
        g_state.mouseY = static_cast<int>(mouseY);
    }

    void SetWindow(GLFWwindow* window)
    {
        g_state.window = window;
    }
} //end namespace nc::input