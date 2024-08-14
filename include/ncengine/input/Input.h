#pragma once

#include "ncmath/Vector.h"

#include <stdint.h>

namespace nc::input
{
    using KeyCode_t = uint32_t;
    enum class KeyCode : KeyCode_t;

    enum class KeyState
    {
        None,
        Pressed,
        Held,
        Released
    };

    auto MouseX() -> uint32_t;
    auto MouseY() -> uint32_t;
    auto MousePos() -> Vector2;
    auto MouseWheel() -> int32_t;

    auto GetXAxis() -> float;  // GetKey A & D; returns -1, 0, 1
    auto GetYAxis() -> float;  // GetKey W & S; returns -1, 0, 1
    auto GetAxis() -> Vector2; // WASD results as a pair

    auto Key(KeyCode keyCode) -> KeyState; // returns KeyState for a key
    auto KeyDown(KeyCode keyCode) -> bool; // true when key is first pressed
    auto KeyUp(KeyCode keyCode) -> bool;   // true when key is released
    auto KeyHeld(KeyCode keyCode) -> bool; // true while key is held

    // See https://www.glfw.org/docs/3.3/group__keys.html
    enum class KeyCode : KeyCode_t
    {
        LeftButton   = 0, RightButton  = 1, MiddleButton = 2, MouseButton4 = 3, 
        MouseButton5 = 4, MouseButton6 = 5, MouseButton7 = 6, MouseButton8 = 7,

        Zero = 48, One = 49, Two   = 50, Three = 51, Four = 52,
        Five = 53, Six = 54, Seven = 55, Eight = 56, Nine = 57,

        Numpad0 = 320,  Numpad1 = 321,  Numpad2 = 322,  Numpad3 = 323,  Numpad4 = 324,
        Numpad5 = 325, Numpad6 = 326, Numpad7 = 327, Numpad8 = 328, Numpad9 = 329,

        F1 = 290, F2 = 291, F3 = 292, F4  = 293, F5  = 294, F6  = 295,
        F7 = 296, F8 = 297, F9 = 298, F10 = 299, F11 = 300, F12 = 301,

        A = 65, B = 66, C = 67, D = 68, E = 69, F = 70, G = 71,
        H = 72, I = 73, J = 74, K = 75, L = 76, M = 77, N = 78,
        O = 79, P = 80, Q = 81, R = 82, S = 83, T = 84, U = 85,
        V = 86, W = 87, X = 88, Y = 89, Z = 90,

        LeftArrow = 263, UpArrow = 265, RightArrow = 262, DownArrow = 264,

        LeftShift = 340, RightShift = 344,
        LeftCtrl  = 341, RightCtrl  = 345,
        LeftAlt   = 342, RightAlt   = 346,

        Space = 32,
        Escape = 256,
        Enter = 257,
        Backspace = 259,
        Tab = 258,
        Tilde = 96,
        CapsLock = 280,
    };
} // namespace nc::input
