#pragma once
#include "win32/NCWin32.h"
#include <stdint.h>
#include <vector>
#include "math/NCVector.h"

namespace nc::input
{
    using VKCode = uint32_t;

    enum class KeyCode : VKCode;

    struct InputItem
    {
        VKCode keyCode;
        LPARAM lparam;

        InputItem(VKCode vkCode, LPARAM param) : keyCode(vkCode), lparam(param) {}
    };

    void SetMouseWheel(WPARAM wParam, LPARAM lParam);
    // void SetLeftButton(WPARAM wParam, LPARAM lParam);
    // void SetMiddleButton(WPARAM wParam, LPARAM lParam);
    // void SetRightButton(WPARAM wParam, LPARAM lParam);
    void ResetMouseState();

    int32_t MouseWheel();
    //bool    LeftButton();
    //bool    MiddleButton();
    //bool    RightButton();
    
    extern std::vector<InputItem> downKeys, upKeys;
    extern uint32_t MouseX, MouseY;
    void UpdateMousePosition(LPARAM lParam);
    void AddToQueue(VKCode vkCode, LPARAM lParam);
    void Flush();

    double GetXAxis();
    double GetYAxis();
    Vector2 GetAxis();

    bool     GetKeyDown(KeyCode keyCode); //true when key is first pressed
    bool     GetKeyUp(KeyCode keyCode);   //true when key is released
    bool     GetKey(KeyCode keyCode);     //true while key is held down

    enum class KeyCode : VKCode
    {
        LeftButton = 1, RightButton = 2, MiddleButton = 4, MouseWheel = 7, //0x07 is undefined so we're stealing it
        
        Zero = 48, One = 49, Two   = 50, Three = 51, Four = 52,
        Five = 53, Six = 54, Seven = 55, Eight = 56, Nine = 57,

        Numpad0 = 96,  Numpad1 = 97,  Numpad2 = 98,  Numpad3 = 99,  Numpad4 = 100,
        Numpad5 = 101, Numpad6 = 102, Numpad7 = 103, Numpad8 = 104, Numpad9 = 105,

        F1 = 112, F2 = 113, F3 = 114, F4  = 115, F5  = 116, F6  = 117,
        F7 = 118, F8 = 119, F9 = 120, F10 = 121, F11 = 122, F12 = 123,       

        A = 65, B = 66, C = 67, D = 68, E = 69, F = 70, G = 71,
        H = 72, I = 73, J = 74, K = 75, L = 76, M = 77, N = 78,
        O = 79, P = 80, Q = 81, R = 82, S = 83, T = 84, U = 85,
        V = 86, W = 87, X = 88, Y = 89, Z = 90,

        LeftArrow = 37, UpArrow = 38, RightArrow = 39, DownArrow = 40,

        Shift = 16, LeftShift = 160, RightShift = 161,
        Ctrl  = 17, LeftCtrl  = 162, RightCtrl  = 163,
        Alt = 18,

        Space = 32,
        Escape = 27,
        Enter = 13,
        Backspace = 8,
        Tab = 9,
        Tilde = 192,
        CapsLock = 20,
    };

} //end namespace nc::input