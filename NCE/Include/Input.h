#ifndef NCE_INPUT
#define NCE_INPUT

#include <windows.h>
#include <windowsx.h>
#include <vector>
#include <iostream>
#include "Vector.h"

namespace nc::input
{
    using VKCode = uint32_t;

    struct InputItem
    {
        VKCode keyCode;
        LPARAM lparam;

        InputItem(VKCode vkCode, LPARAM param) : keyCode(vkCode), lparam(param) {}
    };

    extern std::vector<InputItem> downKeys, upKeys;
    extern uint32_t MouseX, MouseY;
    void UpdateMousePosition(LPARAM lparam_);

    void AddToQueue(VKCode vkCode_, LPARAM lparam_);
    void Flush();

    double GetXAxis();
    double GetYAxis();
    Vector2 GetAxis();

    bool GetKeyDown(VKCode keyCode_); //true when key is first pressed
    bool GetKeyUp(VKCode keyCode_);   //true when key is released
    bool GetKey(VKCode keyCode_);     //true while key is held down

} //end namespace nc::input

#endif