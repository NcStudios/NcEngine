#include "Input.h"
#include <windowsx.h>

namespace nc::input{
    
std::vector<InputItem> downKeys, upKeys;
uint32_t MouseX, MouseY;

void AddToQueue(VKCode vkCode, LPARAM lParam)
{
    //how often is this called unnecessarily?

    bool WasDown = ((lParam & (1 << 30)) != 0);
    bool IsDown  = ((lParam & (1 << 31)) == 0);
    
    if (WasDown)
    {
        if (!IsDown)
        {
            InputItem newItem(vkCode, lParam);
            upKeys.push_back(newItem);
        }
    }
    else if (IsDown)
    {
        InputItem newItem(vkCode, lParam);
        downKeys.push_back(newItem);
    }
}

void Flush()
{
    downKeys.clear();
    upKeys.clear();
}

void UpdateMousePosition(LPARAM lParam)
{
    MouseX = GET_X_LPARAM(lParam); // extracted values can be negative so HI/LO WORD doesn't work
    MouseY = GET_Y_LPARAM(lParam);
}

double GetXAxis() //eventually should bind inputs to axis from config file (controller/WASD/arrows)
{
    double axis = 0.0;
    if (GetKey(KeyCode::D)) { axis += 1.0; }
    if (GetKey(KeyCode::A)) { axis -= 1.0; }
    return axis;
}

double GetYAxis() //eventually binds inputs to axis from config file (controller/WASD/arrows)
{
    double axis = 0.0;
    if (GetKey(KeyCode::W)) { axis -= 1.0; }
    if (GetKey(KeyCode::S)) { axis += 1.0; }
    return axis;
}

Vector2 GetAxis()
{
    return Vector2(GetXAxis(), GetYAxis());
}

bool GetKeyDown(VKCode keyCode)
{
    for(auto item : downKeys)
    {
        if (item.keyCode == (VKCode)keyCode)
        {
            return true;
        }
    }

    return false;
}

bool GetKeyUp(KeyCode keyCode)
{
    for(auto item : upKeys)
    {
        if (item.keyCode == (VKCode)keyCode)
        {
            return true;
        }
    }

    return false;
}

bool GetKey(KeyCode keyCode)
{
    //if most significant bit is 1, key is down
    return (GetAsyncKeyState((VKCode)keyCode) & (1 << 15)) == 0 ? false : true;
}
} //end namespace nc::input