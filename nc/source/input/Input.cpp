#include "Input.h"
#include "InputInternal.h"

#include <algorithm>
#include <vector>
#include <windowsx.h>

namespace nc::input
{
    struct
    {
        std::vector<InputItem> downKeys = {};
        std::vector<InputItem> upKeys = {};
        uint32_t mouseX = 0u;
        uint32_t mouseY = 0u;
        int32_t mouseWheel = 0;
    } g_state;

    uint32_t MouseX() { return g_state.mouseX; }
    uint32_t MouseY() { return g_state.mouseY; }
    Vector2 MousePos() { return Vector2{(float)g_state.mouseX, (float)g_state.mouseY}; }
    int32_t MouseWheel() { return g_state.mouseWheel; }

    float GetXAxis() //eventually should bind inputs to axis from config file (controller/WASD/arrows)
    {
        float axis = 0.0f;
        if (GetKey(KeyCode::D)) { axis += 1.0; }
        if (GetKey(KeyCode::A)) { axis -= 1.0; }
        return axis;
    }

    float GetYAxis() //eventually binds inputs to axis from config file (controller/WASD/arrows)
    {
        float axis = 0.0f;
        if (GetKey(KeyCode::W)) { axis -= 1.0; }
        if (GetKey(KeyCode::S)) { axis += 1.0; }
        return axis;
    }

    Vector2 GetAxis()
    {
        return Vector2(GetXAxis(), GetYAxis());
    }

    bool GetKeyDown(KeyCode keyCode)
    {
        auto beg = g_state.downKeys.cbegin();
        auto end = g_state.downKeys.cend();
        return end != std::find_if(beg, end, [keyCode](const auto& item)
        {
            return item.keyCode == static_cast<KeyCode_t>(keyCode);
        });
    }

    bool GetKeyUp(KeyCode keyCode)
    {
        auto beg = g_state.upKeys.cbegin();
        auto end = g_state.upKeys.cend();
        return end != std::find_if(beg, end, [keyCode](const auto& item)
        {
            return item.keyCode == static_cast<KeyCode_t>(keyCode);
        });
    }

    bool GetKey(KeyCode keyCode)
    {
        //if most significant bit is 1, key is down
        return (GetAsyncKeyState((KeyCode_t)keyCode) & (1 << 15)) == 0 ? false : true;
    }

    void SetMouseWheel(WPARAM wParam, LPARAM)
    {
        g_state.mouseWheel = GET_WHEEL_DELTA_WPARAM(wParam);
    }

    void ResetMouseState()
    {
        g_state.mouseWheel = 0;
    }

    void AddToQueue(KeyCode_t keyCode, LPARAM lParam)
    {
        bool WasDown = ((lParam & (1 << 30)) != 0);
        bool IsDown  = ((lParam & (1 << 31)) == 0);
        
        if (WasDown && !IsDown)
        {
            g_state.upKeys.emplace_back(keyCode, lParam);
        }
        else if (IsDown)
        {
            g_state.downKeys.emplace_back(keyCode, lParam);
        }
    }

    void Flush()
    {
        g_state.downKeys.clear();
        g_state.upKeys.clear();
        ResetMouseState();
    }

    void UpdateMousePosition(LPARAM lParam)
    {
        g_state.mouseX = GET_X_LPARAM(lParam); // extracted values can be negative so HI/LO WORD doesn't work
        g_state.mouseY = GET_Y_LPARAM(lParam);
    }
} //end namespace nc::input