#include "input/Input.h"
#include "InputInternal.h"

#include <algorithm>
#include <vector>
#include <windowsx.h>
#include "GLFW/glfw3.h"

#include <unordered_map>

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
        case GLFW_REPEAT: return KeyState::Held; // Note: 'Repeat' is not strictly the same as 'held', and we handle checking for and setting the 'held' state in Input::Flush. However, we know that if 'repeat' is true, 'held' is also true.
        default: return KeyState::None;
    }
}
}

namespace nc::input
{
    constexpr uint32_t MouseWheelSensitivityScalar = 50u;

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

    void SetMouseWheel(int yOffset)
    {
        g_state.mouseWheel = yOffset * MouseWheelSensitivityScalar;
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
        for (auto cur = g_state.keyStates.begin(); cur != g_state.keyStates.end();)
        {
            auto& state = cur->second;
            if (state == KeyState::Pressed)
            {
                state = KeyState::Held;
            }
            else if (state == KeyState::None)
            {
                g_state.keyStates.erase(cur);
            }
            cur++;
        }
        ResetMouseState();
    }

    void UpdateMousePosition(int mouseX, int mouseY)
    {
        g_state.mouseX = mouseX;
        g_state.mouseY = mouseY;
    }

    void SetWindow(GLFWwindow* window)
    {
        g_state.window = window;
    }
} //end namespace nc::input