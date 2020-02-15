#include "../Include/Input.h"

namespace nc::input{
    
std::vector<InputItem> downKeys, upKeys;
uint32_t MouseX, MouseY;

void AddToQueue(VKCode vkCode_, LPARAM lparam_)
{
    //how often is this called unnecessarily?

    bool WasDown = ((lparam_ & (1 << 30)) != 0);
    bool IsDown  = ((lparam_ & (1 << 31)) == 0);
    
    if (WasDown)
    {
        if (!IsDown)
        {
            InputItem newItem(vkCode_, lparam_);
            upKeys.push_back(newItem);
        }
    }
    else if (IsDown)
    {
        InputItem newItem(vkCode_, lparam_);
        downKeys.push_back(newItem);
    }
}

void Flush()
{
    downKeys.clear();
    upKeys.clear();
}

void UpdateMousePosition(LPARAM lparam_)
{
    MouseX = GET_X_LPARAM(lparam_); // extracted values can be negative so HI/LO WORD doesn't work
    MouseY = GET_Y_LPARAM(lparam_);
}

double GetXAxis() //eventually should bind inputs to axis from config file (controller/WASD/arrows)
{
    double axis = 0.0;
    if (GetKey('D')) { axis += 1.0; }
    if (GetKey('A')) { axis -= 1.0; }
    return axis;
}

double GetYAxis() //eventually binds inputs to axis from config file (controller/WASD/arrows)
{
    double axis = 0.0;
    if (GetKey('W')) { axis -= 1.0; }
    if (GetKey('S')) { axis += 1.0; }
    return axis;
}

bool GetKeyDown(VKCode keyCode_)
{
    for(auto item : downKeys)
    {
        if (item.keyCode == keyCode_)
        {
            return true;
        }
    }

    return false;
}

bool GetKeyUp(VKCode keyCode_)
{
    for(auto item : upKeys)
    {
        if (item.keyCode == keyCode_)
        {
            return true;
        }
    }

    return false;
}

bool GetKey(VKCode keyCode_)
{
    //if most significant bit is 1, key is down
    return (GetAsyncKeyState(keyCode_) & (1 << 15)) == 0 ? false : true;
}
} //end namespace nc::input