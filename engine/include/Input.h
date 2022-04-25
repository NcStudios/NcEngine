#pragma once
#include "math/Vector.h"

#include <stdint.h>

namespace nc::input
{
    using KeyCode_t = uint32_t;
    enum class KeyCode : KeyCode_t;

    auto MouseX() -> uint32_t;
    auto MouseY() -> uint32_t;
    auto MousePos() -> Vector2;
    auto MouseWheel() -> int32_t;

    auto GetXAxis() -> float;  // GetKey A & D; returns -1, 0, 1
    auto GetYAxis() -> float;  // GetKey W & S; returns -1, 0, 1
    auto GetAxis() -> Vector2; // WASD results as a pair

    auto KeyDown(KeyCode keyCode) -> bool; // true when key is first pressed
    auto KeyUp(KeyCode keyCode) -> bool;   // true when key is released
    auto KeyHeld(KeyCode keyCode) -> bool; // true while key is held

    enum class KeyCode : KeyCode_t
    {
        LeftButton = 1, RightButton = 2, MiddleButton = 4, MouseWheel = 7,

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
        Alt   = 18,

        Space = 32,
        Escape = 27,
        Enter = 13,
        Backspace = 8,
        Tab = 9,
        Tilde = 192,
        CapsLock = 20,
    };
} //end namespace nc::input