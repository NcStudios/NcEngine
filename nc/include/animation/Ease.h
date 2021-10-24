#pragma once

#include <cmath>

#include "animation/EasingFunction.h"
#include "animation/EasingModifier.h"


namespace nc
{

    struct Ease
    {
        // http://fooplot.com/#W3sidHlwZSI6MCwiZXEiOiJ4IiwiY29sb3IiOiIjMDAwMDAwIn0seyJ0eXBlIjoxMDAwLCJ3aW5kb3ciOlsiLTAuMTkyNTAwMDAwMDAwMDAwMjUiLCIxLjQzMjQ5OTk5OTk5OTk5OTkiLCIwLjAxNzQ5OTk5OTk5OTk5OTg3NyIsIjEuMDE3NDk5OTk5OTk5OTk5OCJdfV0-
        static const EasingFunction Linear;

        // http://fooplot.com/#W3sidHlwZSI6MCwiZXEiOiIoKDAuMyooMS14KSooMS14KSp4KSsoMy4wKigxLXgpKngqeCkrKHgqeCp4KSkqKDEteCkreCooMS0oMS14KSooMS14KSooMS14KSooMS14KSkiLCJjb2xvciI6IiMwMDAwMDAifSx7InR5cGUiOjEwMDAsIndpbmRvdyI6WyItMC4xOTI1MDAwMDAwMDAwMDAyNSIsIjEuNDMyNDk5OTk5OTk5OTk5OSIsIjAuMDE3NDk5OTk5OTk5OTk5ODc3IiwiMS4wMTc0OTk5OTk5OTk5OTk4Il19XQ--
        static const EasingFunction Standard;

        // http://fooplot.com/#W3sidHlwZSI6MCwiZXEiOiJ4KngiLCJjb2xvciI6IiMwMDAwMDAifSx7InR5cGUiOjEwMDAsIndpbmRvdyI6WyItMC4xOTI1MDAwMDAwMDAwMDAyNSIsIjEuNDMyNDk5OTk5OTk5OTk5OSIsIjAuMDE3NDk5OTk5OTk5OTk5ODc3IiwiMS4wMTc0OTk5OTk5OTk5OTk4Il19XQ--
        static const EasingFunction Quad;

        // http://fooplot.com/#W3sidHlwZSI6MCwiZXEiOiJ4KngqeCIsImNvbG9yIjoiIzAwMDAwMCJ9LHsidHlwZSI6MTAwMCwid2luZG93IjpbIi0wLjE5MjUwMDAwMDAwMDAwMDI1IiwiMS40MzI0OTk5OTk5OTk5OTk5IiwiMC4wMTc0OTk5OTk5OTk5OTk4NzciLCIxLjAxNzQ5OTk5OTk5OTk5OTgiXX1d
        static const EasingFunction Cubic;

        // http://fooplot.com/#W3sidHlwZSI6MCwiZXEiOiJ4KngqeCp4IiwiY29sb3IiOiIjMDAwMDAwIn0seyJ0eXBlIjoxMDAwLCJ3aW5kb3ciOlsiLTAuMTkyNTAwMDAwMDAwMDAwMjUiLCIxLjQzMjQ5OTk5OTk5OTk5OTkiLCIwLjAxNzQ5OTk5OTk5OTk5OTg3NyIsIjEuMDE3NDk5OTk5OTk5OTk5OCJdfV0-
        static const EasingFunction Quartic;

        // http://fooplot.com/#W3sidHlwZSI6MCwiZXEiOiJ4KngqeCp4KngiLCJjb2xvciI6IiMwMDAwMDAifSx7InR5cGUiOjEwMDAsIndpbmRvdyI6WyItMC4xOTI1MDAwMDAwMDAwMDAyNSIsIjEuNDMyNDk5OTk5OTk5OTk5OSIsIjAuMDE3NDk5OTk5OTk5OTk5ODc3IiwiMS4wMTc0OTk5OTk5OTk5OTk4Il19XQ--
        static const EasingFunction Quintic;

        // http://fooplot.com/#W3sidHlwZSI6MCwiZXEiOiIoeCp4KngpKyh4KngpLXgiLCJjb2xvciI6IiMwMDAwMDAifSx7InR5cGUiOjEwMDAsIndpbmRvdyI6WyItMC4xOTI1MDAwMDAwMDAwMDAyNSIsIjEuNDMyNDk5OTk5OTk5OTk5OSIsIjAuMDE3NDk5OTk5OTk5OTk5ODc3IiwiMS4wMTc0OTk5OTk5OTk5OTk4Il19XQ--
        static const EasingFunction Back;

        // http://fooplot.com/#W3sidHlwZSI6MCwiZXEiOiJzaW4oeCoxLjU3MDc5NjMyNjc5KSIsImNvbG9yIjoiIzAwMDAwMCJ9LHsidHlwZSI6MTAwMCwid2luZG93IjpbIi0wLjE5MjUwMDAwMDAwMDAwMDI1IiwiMS40MzI0OTk5OTk5OTk5OTk5IiwiMC4wMTc0OTk5OTk5OTk5OTk4NzciLCIxLjAxNzQ5OTk5OTk5OTk5OTgiXX1d
        static const EasingFunction Sine;

        // http://fooplot.com/#W3sidHlwZSI6MCwiZXEiOiIoMS4wLXgqKDcuMC8xMCkpKngqKDEwLjAvMy4wKSIsImNvbG9yIjoiIzAwMDAwMCJ9LHsidHlwZSI6MTAwMCwid2luZG93IjpbIi0wLjE5MjUwMDAwMDAwMDAwMDI1IiwiMS40MzI0OTk5OTk5OTk5OTk5IiwiMC4wMTc0OTk5OTk5OTk5OTk4NzciLCIxLjAxNzQ5OTk5OTk5OTk5OTgiXX1d
        static const EasingFunction Overshot;

        // http://fooplot.com/#W3sidHlwZSI6MCwiZXEiOiIoeCp4KigoMi4wKngqeCp4KSt4KngtKDQuMCp4KSsyLjApKSotc2luKHgqMTAuOTk1NTc0Mjg3NikiLCJjb2xvciI6IiMwMDAwMDAifSx7InR5cGUiOjEwMDAsIndpbmRvdyI6WyItMC4xOTI1MDAwMDAwMDAwMDAyNSIsIjEuNDMyNDk5OTk5OTk5OTk5OSIsIjAuMDE3NDk5OTk5OTk5OTk5ODc3IiwiMS4wMTc0OTk5OTk5OTk5OTk4Il19XQ--
        static const EasingFunction Elastic;

        // http://fooplot.com/#W3sidHlwZSI6MCwiZXEiOiJhYnMoeC1zaW4oeCozLjE0MTU5MjY1MzU5KSkiLCJjb2xvciI6IiMwMDAwMDAifSx7InR5cGUiOjEwMDAsIndpbmRvdyI6WyItMC4xOTI1MDAwMDAwMDAwMDAyNSIsIjEuNDMyNDk5OTk5OTk5OTk5OSIsIjAuMDE3NDk5OTk5OTk5OTk5ODc3IiwiMS4wMTc0OTk5OTk5OTk5OTk4Il19XQ--
        static const EasingFunction Revisit;

        // http://fooplot.com/#W3sidHlwZSI6MCwiZXEiOiIoMS4wLWNvcyh4KngqeCozNi4wKSooMS4wLXgpKSIsImNvbG9yIjoiIzAwMDAwMCJ9LHsidHlwZSI6MTAwMCwid2luZG93IjpbIi0wLjE5MjUwMDAwMDAwMDAwMDI1IiwiMS40MzI0OTk5OTk5OTk5OTk5IiwiMC4wMTc0OTk5OTk5OTk5OTk4NzciLCIxLjAxNzQ5OTk5OTk5OTk5OTgiXX1d
        static const EasingFunction Lasso;

        // http://fooplot.com/#W3sidHlwZSI6MCwiZXEiOiIoMS4wLWFicygoMS4wLXgqeCkqY29zKHgqeCp4KjE0LjgwNDQwNjYwMTYpKSkiLCJjb2xvciI6IiMwMDAwMDAifSx7InR5cGUiOjEwMDAsIndpbmRvdyI6WyItMC4xOTI1MDAwMDAwMDAwMDAyNSIsIjEuNDMyNDk5OTk5OTk5OTk5OSIsIjAuMDE3NDk5OTk5OTk5OTk5ODc3IiwiMS4wMTc0OTk5OTk5OTk5OTk4Il19XQ--
        static const EasingFunction SlowBounce;

        // http://fooplot.com/#W3sidHlwZSI6MCwiZXEiOiIoMS4wLWFicygoMS4wLXgpKmNvcyh4KngqMTQuODA0NDA2NjAxNikpKSIsImNvbG9yIjoiIzAwMDAwMCJ9LHsidHlwZSI6MTAwMCwid2luZG93IjpbIi0wLjE5MjUwMDAwMDAwMDAwMDI1IiwiMS40MzI0OTk5OTk5OTk5OTk5IiwiMC4wMTc0OTk5OTk5OTk5OTk4NzciLCIxLjAxNzQ5OTk5OTk5OTk5OTgiXX1d
        static const EasingFunction Bounce;

        // http://fooplot.com/#W3sidHlwZSI6MCwiZXEiOiIoMS4wLWFicygoMS14KSooMS14KSpjb3MoeCp4KjE0LjgwNDQwNjYwMTYpKSkiLCJjb2xvciI6IiMwMDAwMDAifSx7InR5cGUiOjEwMDAsIndpbmRvdyI6WyItMC4xOTI1MDAwMDAwMDAwMDAyNSIsIjEuNDMyNDk5OTk5OTk5OTk5OSIsIjAuMDE3NDk5OTk5OTk5OTk5ODc3IiwiMS4wMTc0OTk5OTk5OTk5OTk4Il19XQ--
        static const EasingFunction SmallBounce;

        // http://fooplot.com/#W3sidHlwZSI6MCwiZXEiOiIoMS4wLWFicygoMS14KSooMS14KSpjb3MoeCp4KjE0LjgwNDQwNjYwMTYpKSkiLCJjb2xvciI6IiMwMDAwMDAifSx7InR5cGUiOjEwMDAsIndpbmRvdyI6WyItMC4xOTI1MDAwMDAwMDAwMDAyNSIsIjEuNDMyNDk5OTk5OTk5OTk5OSIsIjAuMDE3NDk5OTk5OTk5OTk5ODc3IiwiMS4wMTc0OTk5OTk5OTk5OTk4Il19XQ--
        static const EasingFunction TinyBounce;

        // http://fooplot.com/#W3sidHlwZSI6MCwiZXEiOiIoY29zKHgqeCoxMi4wKSp4KigxLjAteCkreCkiLCJjb2xvciI6IiMwMDAwMDAifSx7InR5cGUiOjEwMDAsIndpbmRvdyI6WyItMC4xOTI1MDAwMDAwMDAwMDAyNSIsIjEuNDMyNDk5OTk5OTk5OTk5OSIsIjAuMDE3NDk5OTk5OTk5OTk5ODc3IiwiMS4wMTc0OTk5OTk5OTk5OTk4Il19XQ--
        static const EasingFunction Hesitant;

        // http://fooplot.com/#W3sidHlwZSI6MCwiZXEiOiJzcXJ0KHgpIiwiY29sb3IiOiIjMDAwMDAwIn0seyJ0eXBlIjoxMDAwLCJ3aW5kb3ciOlsiLTAuMTkyNTAwMDAwMDAwMDAwMjUiLCIxLjQzMjQ5OTk5OTk5OTk5OTkiLCIwLjAxNzQ5OTk5OTk5OTk5OTg3NyIsIjEuMDE3NDk5OTk5OTk5OTk5OCJdfV0-
        static const EasingFunction Sqrt;

        // http://fooplot.com/#W3sidHlwZSI6MCwiZXEiOiIoKDEuMC0oMS14KSooMS14KSooMS14KSooMS14KSkreCkqMC41IiwiY29sb3IiOiIjMDAwMDAwIn0seyJ0eXBlIjoxMDAwLCJ3aW5kb3ciOlsiLTAuMTkyNTAwMDAwMDAwMDAwMjUiLCIxLjQzMjQ5OTk5OTk5OTk5OTkiLCIwLjAxNzQ5OTk5OTk5OTk5OTg3NyIsIjEuMDE3NDk5OTk5OTk5OTk5OCJdfV0-
        static const EasingFunction Sqrtf;

        // http://fooplot.com/#W3sidHlwZSI6MCwiZXEiOiIobG9nKHgpKzIuMCkqMC41IiwiY29sb3IiOiIjMDAwMDAwIn0seyJ0eXBlIjoxMDAwLCJ3aW5kb3ciOlsiLTAuMTkyNTAwMDAwMDAwMDAwMjUiLCIxLjQzMjQ5OTk5OTk5OTk5OTkiLCIwLjAxNzQ5OTk5OTk5OTk5OTg3NyIsIjEuMDE3NDk5OTk5OTk5OTk5OCJdfV0-
        static const EasingFunction Log10;

        // http://fooplot.com/#W3sidHlwZSI6MCwiZXEiOiIoeDwwLjc_KHgqLTAuMzU3KTooKCh4LTAuNykqKHgtMC43KSoyNy41LTAuNSkqMC41KSkiLCJjb2xvciI6IiMwMDAwMDAifSx7InR5cGUiOjEwMDAsIndpbmRvdyI6WyItMC4xOTI1MDAwMDAwMDAwMDAyNSIsIjEuNDMyNDk5OTk5OTk5OTk5OSIsIjAuMDE3NDk5OTk5OTk5OTk5ODc3IiwiMS4wMTc0OTk5OTk5OTk5OTk4Il19XQ--
        static const EasingFunction Slingshot;

        // http://fooplot.com/#W3sidHlwZSI6MCwiZXEiOiIxLXNxcnQoMS14KngpIiwiY29sb3IiOiIjMDAwMDAwIn0seyJ0eXBlIjoxMDAwLCJ3aW5kb3ciOlsiLTAuMTkyNTAwMDAwMDAwMDAwMjUiLCIxLjQzMjQ5OTk5OTk5OTk5OTkiLCIwLjAxNzQ5OTk5OTk5OTk5OTg3NyIsIjEuMDE3NDk5OTk5OTk5OTk5OCJdfV0-
        static const EasingFunction Circular;

        // http://fooplot.com/#W3sidHlwZSI6MCwiZXEiOiIoMy4wKigxLjAteCkqeCp4KSsoeCp4KngpIiwiY29sb3IiOiIjMDAwMDAwIn0seyJ0eXBlIjoxMDAwLCJ3aW5kb3ciOlsiLTAuMTkyNTAwMDAwMDAwMDAwMjUiLCIxLjQzMjQ5OTk5OTk5OTk5OTkiLCIwLjAxNzQ5OTk5OTk5OTk5OTg3NyIsIjEuMDE3NDk5OTk5OTk5OTk5OCJdfV0-
        static const EasingFunction Gentle;


        /** Easing Modifiers */
        
        static const EasingModifierFactory In;
        
        static const EasingModifierFactory Out;
        
        static const EasingModifierFactory InOut;
        
        static const EasingModifierFactory YoYo;
        
        static const EasingModifierFactory Mirror;
        
        static const EasingModifierFactory Reverse;
        
        static const EasingModifierFactory Flip;
    };
    
    

}