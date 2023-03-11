#include "input/Input.h"
#include "InputInternal.h"

#include <algorithm>
#include <vector>
#include <windowsx.h>
#include "GLFW/glfw3.h"

namespace nc::input
{
    struct
    {
        std::vector<InputItem> downKeys = {};
        std::vector<InputItem> upKeys = {};
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
        auto beg = g_state.downKeys.cbegin();
        auto end = g_state.downKeys.cend();
        return end != std::ranges::find_if(beg, end, [keyCode](const auto& item)
        {
            return item.keyCode == static_cast<KeyCode_t>(keyCode);
        });
    }

    bool KeyUp(KeyCode keyCode)
    {
        auto beg = g_state.upKeys.cbegin();
        auto end = g_state.upKeys.cend();
        return end != std::find_if(beg, end, [keyCode](const auto& item)
        {
            return item.keyCode == static_cast<KeyCode_t>(keyCode);
        });
    }

    bool KeyHeld(KeyCode keyCode)
    {
        //if most significant bit is 1, key is down
        auto state =  glfwGetKey(g_state.window, (KeyCode_t)keyCode);
        return state == GLFW_REPEAT;
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
        if (action == GLFW_RELEASE)
        {
            g_state.upKeys.emplace_back(keyCode, action);
        }
        else if (action == GLFW_PRESS)
        {
            g_state.downKeys.emplace_back(keyCode, action);
        }
    }

    void AddMouseButtonDownToQueue(KeyCode_t keyCode, int action)
    {
        g_state.downKeys.emplace_back(keyCode, action);
    }

    void AddMouseButtonUpToQueue(KeyCode_t keyCode, int action)
    {
        g_state.upKeys.emplace_back(keyCode, action);
    }

    void Flush()
    {
        g_state.downKeys.clear();
        g_state.upKeys.clear();
        ResetMouseState();
    }

    void UpdateMousePosition(int mouseX, int mouseY)
    {
        g_state.mouseX = mouseX;
        g_state.mouseY = static_cast<int>(mouseY); // Convert to top-left coordinates
        //g_state.mouseY = static_cast<int>(mouseY * 0.5 + 0.5); // Convert to top-left coordinates
    }

    void SetWindow(GLFWwindow* window)
    {
        g_state.window = window;
    }
} //end namespace nc::input