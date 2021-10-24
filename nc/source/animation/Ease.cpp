
#include "animation/Ease.h"

namespace nc
{

    const EasingFunction Ease::Linear("Linear", [](float x) {
        return x;
    });

    // http://fooplot.com/#W3sidHlwZSI6MCwiZXEiOiIoKDAuMyooMS14KSooMS14KSp4KSsoMy4wKigxLXgpKngqeCkrKHgqeCp4KSkqKDEteCkreCooMS0oMS14KSooMS14KSooMS14KSooMS14KSkiLCJjb2xvciI6IiMwMDAwMDAifSx7InR5cGUiOjEwMDAsIndpbmRvdyI6WyItMC4xOTI1MDAwMDAwMDAwMDAyNSIsIjEuNDMyNDk5OTk5OTk5OTk5OSIsIjAuMDE3NDk5OTk5OTk5OTk5ODc3IiwiMS4wMTc0OTk5OTk5OTk5OTk4Il19XQ--
    const EasingFunction Ease::Standard("Standard", [](float x) {
        float i = (1.0f - x);
        float i2 = i * i;
        float x2 = x * x;
        float eq1 = (0.3f * i2 * x) + (3.0f * i * x2) + (x2 * x);
        float eq2 = 1.0f - i2 * i2;
        return eq1 * i + eq2 * x;
    });

    // http://fooplot.com/#W3sidHlwZSI6MCwiZXEiOiJ4KngiLCJjb2xvciI6IiMwMDAwMDAifSx7InR5cGUiOjEwMDAsIndpbmRvdyI6WyItMC4xOTI1MDAwMDAwMDAwMDAyNSIsIjEuNDMyNDk5OTk5OTk5OTk5OSIsIjAuMDE3NDk5OTk5OTk5OTk5ODc3IiwiMS4wMTc0OTk5OTk5OTk5OTk4Il19XQ--
    const EasingFunction Ease::Quad("Quad", [](float x) {
        return x * x;
    });

    // http://fooplot.com/#W3sidHlwZSI6MCwiZXEiOiJ4KngqeCIsImNvbG9yIjoiIzAwMDAwMCJ9LHsidHlwZSI6MTAwMCwid2luZG93IjpbIi0wLjE5MjUwMDAwMDAwMDAwMDI1IiwiMS40MzI0OTk5OTk5OTk5OTk5IiwiMC4wMTc0OTk5OTk5OTk5OTk4NzciLCIxLjAxNzQ5OTk5OTk5OTk5OTgiXX1d
    const EasingFunction Ease::Cubic("Cubic", [](float x) {
        return x * x * x;
    });

    // http://fooplot.com/#W3sidHlwZSI6MCwiZXEiOiJ4KngqeCp4IiwiY29sb3IiOiIjMDAwMDAwIn0seyJ0eXBlIjoxMDAwLCJ3aW5kb3ciOlsiLTAuMTkyNTAwMDAwMDAwMDAwMjUiLCIxLjQzMjQ5OTk5OTk5OTk5OTkiLCIwLjAxNzQ5OTk5OTk5OTk5OTg3NyIsIjEuMDE3NDk5OTk5OTk5OTk5OCJdfV0-
    const EasingFunction Ease::Quartic("Quartic", [](float x) {
        float x2 = x * x;
        return x2 * x2;
    });

    // http://fooplot.com/#W3sidHlwZSI6MCwiZXEiOiJ4KngqeCp4KngiLCJjb2xvciI6IiMwMDAwMDAifSx7InR5cGUiOjEwMDAsIndpbmRvdyI6WyItMC4xOTI1MDAwMDAwMDAwMDAyNSIsIjEuNDMyNDk5OTk5OTk5OTk5OSIsIjAuMDE3NDk5OTk5OTk5OTk5ODc3IiwiMS4wMTc0OTk5OTk5OTk5OTk4Il19XQ--
    const EasingFunction Ease::Quintic("Quintic", [](float x) {
        float x2 = x * x;
        return x2 * x2 * x;
    });

    // http://fooplot.com/#W3sidHlwZSI6MCwiZXEiOiIoeCp4KngpKyh4KngpLXgiLCJjb2xvciI6IiMwMDAwMDAifSx7InR5cGUiOjEwMDAsIndpbmRvdyI6WyItMC4xOTI1MDAwMDAwMDAwMDAyNSIsIjEuNDMyNDk5OTk5OTk5OTk5OSIsIjAuMDE3NDk5OTk5OTk5OTk5ODc3IiwiMS4wMTc0OTk5OTk5OTk5OTk4Il19XQ--
    const EasingFunction Ease::Back("Back", [](float x) {
        float x2 = x * x;
        float x3 = x2 * x;
        return x3 + x2 - x;
    });

    // http://fooplot.com/#W3sidHlwZSI6MCwiZXEiOiJzaW4oeCoxLjU3MDc5NjMyNjc5KSIsImNvbG9yIjoiIzAwMDAwMCJ9LHsidHlwZSI6MTAwMCwid2luZG93IjpbIi0wLjE5MjUwMDAwMDAwMDAwMDI1IiwiMS40MzI0OTk5OTk5OTk5OTk5IiwiMC4wMTc0OTk5OTk5OTk5OTk4NzciLCIxLjAxNzQ5OTk5OTk5OTk5OTgiXX1d
    const EasingFunction Ease::Sine("Sine", [](float x) {
        return sin(x * 1.57079632679f);
    });

    // http://fooplot.com/#W3sidHlwZSI6MCwiZXEiOiIoMS4wLXgqKDcuMC8xMCkpKngqKDEwLjAvMy4wKSIsImNvbG9yIjoiIzAwMDAwMCJ9LHsidHlwZSI6MTAwMCwid2luZG93IjpbIi0wLjE5MjUwMDAwMDAwMDAwMDI1IiwiMS40MzI0OTk5OTk5OTk5OTk5IiwiMC4wMTc0OTk5OTk5OTk5OTk4NzciLCIxLjAxNzQ5OTk5OTk5OTk5OTgiXX1d
    const EasingFunction Ease::Overshot("Overshot", [](float x) {
        return (1.0f - x * (7.0f / 10.0f)) * x * (10.0f / 3.0f);
    });

    // http://fooplot.com/#W3sidHlwZSI6MCwiZXEiOiIoeCp4KigoMi4wKngqeCp4KSt4KngtKDQuMCp4KSsyLjApKSotc2luKHgqMTAuOTk1NTc0Mjg3NikiLCJjb2xvciI6IiMwMDAwMDAifSx7InR5cGUiOjEwMDAsIndpbmRvdyI6WyItMC4xOTI1MDAwMDAwMDAwMDAyNSIsIjEuNDMyNDk5OTk5OTk5OTk5OSIsIjAuMDE3NDk5OTk5OTk5OTk5ODc3IiwiMS4wMTc0OTk5OTk5OTk5OTk4Il19XQ--
    const EasingFunction Ease::Elastic("Elastic", [](float x) {
        float x2 = x * x;
        float x3 = x2 * x;
        float scale = x2 * ((2.0f * x3) + x2 - (4.0f * x) + 2.0f);
        float wave = -sin(x * 10.9955742876f);
        return scale * wave;
    });

    // http://fooplot.com/#W3sidHlwZSI6MCwiZXEiOiJhYnMoeC1zaW4oeCozLjE0MTU5MjY1MzU5KSkiLCJjb2xvciI6IiMwMDAwMDAifSx7InR5cGUiOjEwMDAsIndpbmRvdyI6WyItMC4xOTI1MDAwMDAwMDAwMDAyNSIsIjEuNDMyNDk5OTk5OTk5OTk5OSIsIjAuMDE3NDk5OTk5OTk5OTk5ODc3IiwiMS4wMTc0OTk5OTk5OTk5OTk4Il19XQ--
    const EasingFunction Ease::Revisit("Revisit", [](float x) {
        return abs( x - sin(x * 3.14159265359f) );
    });

    // http://fooplot.com/#W3sidHlwZSI6MCwiZXEiOiIoMS4wLWNvcyh4KngqeCozNi4wKSooMS4wLXgpKSIsImNvbG9yIjoiIzAwMDAwMCJ9LHsidHlwZSI6MTAwMCwid2luZG93IjpbIi0wLjE5MjUwMDAwMDAwMDAwMDI1IiwiMS40MzI0OTk5OTk5OTk5OTk5IiwiMC4wMTc0OTk5OTk5OTk5OTk4NzciLCIxLjAxNzQ5OTk5OTk5OTk5OTgiXX1d
    const EasingFunction Ease::Lasso("Lasso", [](float x) {
        return (1.0f - cos(x * x * x * 36.0f) * (1.0f - x));
    });

    // http://fooplot.com/#W3sidHlwZSI6MCwiZXEiOiIoMS4wLWFicygoMS4wLXgqeCkqY29zKHgqeCp4KjE0LjgwNDQwNjYwMTYpKSkiLCJjb2xvciI6IiMwMDAwMDAifSx7InR5cGUiOjEwMDAsIndpbmRvdyI6WyItMC4xOTI1MDAwMDAwMDAwMDAyNSIsIjEuNDMyNDk5OTk5OTk5OTk5OSIsIjAuMDE3NDk5OTk5OTk5OTk5ODc3IiwiMS4wMTc0OTk5OTk5OTk5OTk4Il19XQ--
    const EasingFunction Ease::SlowBounce("SlowBounce", [](float x) {
        float x2 = x * x;
        return (1.0f - abs((1.0f - x2) * cos(x2 * x * 14.8044066016f)));
    });

    // http://fooplot.com/#W3sidHlwZSI6MCwiZXEiOiIoMS4wLWFicygoMS4wLXgpKmNvcyh4KngqMTQuODA0NDA2NjAxNikpKSIsImNvbG9yIjoiIzAwMDAwMCJ9LHsidHlwZSI6MTAwMCwid2luZG93IjpbIi0wLjE5MjUwMDAwMDAwMDAwMDI1IiwiMS40MzI0OTk5OTk5OTk5OTk5IiwiMC4wMTc0OTk5OTk5OTk5OTk4NzciLCIxLjAxNzQ5OTk5OTk5OTk5OTgiXX1d
    const EasingFunction Ease::Bounce("Bounce", [](float x) {
        return (1.0f - abs((1.0f - x) * cos(x * x * 14.8044066016f)));
    });

    // http://fooplot.com/#W3sidHlwZSI6MCwiZXEiOiIoMS4wLWFicygoMS14KSooMS14KSpjb3MoeCp4KjE0LjgwNDQwNjYwMTYpKSkiLCJjb2xvciI6IiMwMDAwMDAifSx7InR5cGUiOjEwMDAsIndpbmRvdyI6WyItMC4xOTI1MDAwMDAwMDAwMDAyNSIsIjEuNDMyNDk5OTk5OTk5OTk5OSIsIjAuMDE3NDk5OTk5OTk5OTk5ODc3IiwiMS4wMTc0OTk5OTk5OTk5OTk4Il19XQ--
    const EasingFunction Ease::SmallBounce("SmallBounce", [](float x) {
        float inv = 1.0f - x;
        return (1.0f - abs(inv * inv * cos(x * x * 14.8044066016f)));
    });

    // http://fooplot.com/#W3sidHlwZSI6MCwiZXEiOiIoMS4wLWFicygoMS14KSooMS14KSpjb3MoeCp4KjE0LjgwNDQwNjYwMTYpKSkiLCJjb2xvciI6IiMwMDAwMDAifSx7InR5cGUiOjEwMDAsIndpbmRvdyI6WyItMC4xOTI1MDAwMDAwMDAwMDAyNSIsIjEuNDMyNDk5OTk5OTk5OTk5OSIsIjAuMDE3NDk5OTk5OTk5OTk5ODc3IiwiMS4wMTc0OTk5OTk5OTk5OTk4Il19XQ--
    const EasingFunction Ease::TinyBounce("TinyBounce", [](float x) {
        float inv = 1.0f - x;
        return (1.0 - abs(inv * inv * cos(x * x * 7.0)));
    });

    // http://fooplot.com/#W3sidHlwZSI6MCwiZXEiOiIoY29zKHgqeCoxMi4wKSp4KigxLjAteCkreCkiLCJjb2xvciI6IiMwMDAwMDAifSx7InR5cGUiOjEwMDAsIndpbmRvdyI6WyItMC4xOTI1MDAwMDAwMDAwMDAyNSIsIjEuNDMyNDk5OTk5OTk5OTk5OSIsIjAuMDE3NDk5OTk5OTk5OTk5ODc3IiwiMS4wMTc0OTk5OTk5OTk5OTk4Il19XQ--
    const EasingFunction Ease::Hesitant("Hesitant", [](float x) {
        return (cos(x * x * 12.0f) * x * (1.0f - x) + x);
    });

    // http://fooplot.com/#W3sidHlwZSI6MCwiZXEiOiJzcXJ0KHgpIiwiY29sb3IiOiIjMDAwMDAwIn0seyJ0eXBlIjoxMDAwLCJ3aW5kb3ciOlsiLTAuMTkyNTAwMDAwMDAwMDAwMjUiLCIxLjQzMjQ5OTk5OTk5OTk5OTkiLCIwLjAxNzQ5OTk5OTk5OTk5OTg3NyIsIjEuMDE3NDk5OTk5OTk5OTk5OCJdfV0-
    const EasingFunction Ease::Sqrt("Sqrt", [](float x) {
        return sqrt(x);
    });

    // http://fooplot.com/#W3sidHlwZSI6MCwiZXEiOiIoKDEuMC0oMS14KSooMS14KSooMS14KSooMS14KSkreCkqMC41IiwiY29sb3IiOiIjMDAwMDAwIn0seyJ0eXBlIjoxMDAwLCJ3aW5kb3ciOlsiLTAuMTkyNTAwMDAwMDAwMDAwMjUiLCIxLjQzMjQ5OTk5OTk5OTk5OTkiLCIwLjAxNzQ5OTk5OTk5OTk5OTg3NyIsIjEuMDE3NDk5OTk5OTk5OTk5OCJdfV0-
    const EasingFunction Ease::Sqrtf("Sqrtf", [](float x) {
        float i = (1.0f - x);
        float i2 = i * i;
        return ((1.0f - i2 * i2) + x) * 0.5f;
    });

    // http://fooplot.com/#W3sidHlwZSI6MCwiZXEiOiIobG9nKHgpKzIuMCkqMC41IiwiY29sb3IiOiIjMDAwMDAwIn0seyJ0eXBlIjoxMDAwLCJ3aW5kb3ciOlsiLTAuMTkyNTAwMDAwMDAwMDAwMjUiLCIxLjQzMjQ5OTk5OTk5OTk5OTkiLCIwLjAxNzQ5OTk5OTk5OTk5OTg3NyIsIjEuMDE3NDk5OTk5OTk5OTk5OCJdfV0-
    const EasingFunction Ease::Log10("Log10", [](float x) {
        return (log10(x + 0.01f) + 2.0f) * 0.5f / 1.0021606868913213f;
    });

    // http://fooplot.com/#W3sidHlwZSI6MCwiZXEiOiIoeDwwLjc_KHgqLTAuMzU3KTooKCh4LTAuNykqKHgtMC43KSoyNy41LTAuNSkqMC41KSkiLCJjb2xvciI6IiMwMDAwMDAifSx7InR5cGUiOjEwMDAsIndpbmRvdyI6WyItMC4xOTI1MDAwMDAwMDAwMDAyNSIsIjEuNDMyNDk5OTk5OTk5OTk5OSIsIjAuMDE3NDk5OTk5OTk5OTk5ODc3IiwiMS4wMTc0OTk5OTk5OTk5OTk4Il19XQ--
    const EasingFunction Ease::Slingshot("Slingshot", [](float x) {
        if (x < 0.7f) {
            return (x * -0.357f);
        } else {
            float d = x - 0.7f;
            return ((d * d * 27.5f - 0.5f) * 0.5f);
        }
    });

    // http://fooplot.com/#W3sidHlwZSI6MCwiZXEiOiIxLXNxcnQoMS14KngpIiwiY29sb3IiOiIjMDAwMDAwIn0seyJ0eXBlIjoxMDAwLCJ3aW5kb3ciOlsiLTAuMTkyNTAwMDAwMDAwMDAwMjUiLCIxLjQzMjQ5OTk5OTk5OTk5OTkiLCIwLjAxNzQ5OTk5OTk5OTk5OTg3NyIsIjEuMDE3NDk5OTk5OTk5OTk5OCJdfV0-
    const EasingFunction Ease::Circular("Circular", [](float x) {
        return 1.0f - sqrt( 1.0f - x * x );
    });

    // http://fooplot.com/#W3sidHlwZSI6MCwiZXEiOiIoMy4wKigxLjAteCkqeCp4KSsoeCp4KngpIiwiY29sb3IiOiIjMDAwMDAwIn0seyJ0eXBlIjoxMDAwLCJ3aW5kb3ciOlsiLTAuMTkyNTAwMDAwMDAwMDAwMjUiLCIxLjQzMjQ5OTk5OTk5OTk5OTkiLCIwLjAxNzQ5OTk5OTk5OTk5OTg3NyIsIjEuMDE3NDk5OTk5OTk5OTk5OCJdfV0-
    const EasingFunction Ease::Gentle("Gentle", [](float x) {
        return (3.0f * (1.0f - x) * x * x) + (x * x * x);
    });


    /** Easing Modifiers */
    
    const EasingModifierFactory Ease::In = EasingModifier::Factory("In", [](Easing* ease) {
        return [ease](float x) {
            return ease->Compute(x);
        };
    });
    
    const EasingModifierFactory Ease::Out = EasingModifier::Factory("Out", [](Easing* ease) {
        return [ease](float x) {
            return 1.0f - ease->Compute( 1.0f - x );
        };
    });
    
    const EasingModifierFactory Ease::InOut = EasingModifier::Factory("InOut", [](Easing* ease) {
        return [ease](float x) {
            return x < 0.5f
            ? ease->Compute( 2.0f * x ) * 0.5f
            : 1.0f - (ease->Compute( 2.0f - 2.0f * x ) * 0.5f);
        };
    });
    
    const EasingModifierFactory Ease::YoYo = EasingModifier::Factory("YoYo", [](Easing* ease) {
        return [ease](float x) {
            return x < 0.5f
                ? ease->Compute( 2.0f * x )
                : ease->Compute( 2.0f - 2.0f * x );
        };
    });
    
    const EasingModifierFactory Ease::Mirror = EasingModifier::Factory("Mirror", [](Easing* ease) {
        return [ease](float x) {
            return x < 0.5f
                ? ease->Compute( 2.0f * x )
                : 1.0 - ease->Compute( 2.0f - 2.0f * x );
        };
    });
    
    const EasingModifierFactory Ease::Reverse = EasingModifier::Factory("Reverse", [](Easing* ease) {
        return [ease](float x) {
            return ease->Compute( 1.0f - x );
        };
    });
    
    const EasingModifierFactory Ease::Flip = EasingModifier::Factory("Flip", [](Easing* ease) {
        return [ease](float x) {
            return 1.0f - ease->Compute( x );
        };
    });

}