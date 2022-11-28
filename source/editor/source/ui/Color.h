#pragma once

#include "ncmath/Vector.h"

namespace nc::editor
{
    struct Color
    {
        static constexpr auto Black             = Vector4{0.009f, 0.009f, 0.009f, 1.000f};
        static constexpr auto White             = Vector4{1.000f, 1.000f, 1.000f, 1.000f};
        static constexpr auto Clear             = Vector4{0.000f, 0.000f, 0.000f, 0.000f};
        static constexpr auto TransWhite        = Vector4{1.000f, 1.000f, 1.000f, 0.700f};

        static constexpr auto Red               = Vector4{1.000f, 0.200f, 0.100f, 1.000f};
        static constexpr auto Green             = Vector4{0.000f, 1.000f, 0.000f, 1.000f};
        static constexpr auto Blue              = Vector4{0.000f, 0.400f, 1.000f, 1.000f};

        static constexpr auto GrayLightest      = Vector4{0.348f, 0.348f, 0.348f, 1.000f};
        static constexpr auto GrayLight         = Vector4{0.168f, 0.168f, 0.168f, 1.000f};
        static constexpr auto Gray              = Vector4{0.072f, 0.072f, 0.072f, 1.000f};
        static constexpr auto GrayDark          = Vector4{0.035f, 0.035f, 0.035f, 1.000f};
        static constexpr auto GrayDarkest       = Vector4{0.018f, 0.018f, 0.018f, 1.000f};

        static constexpr auto TransGrayLight    = Vector4{0.740f, 0.740f, 0.740f, 0.200f};
        static constexpr auto TransGrayLightAlt = Vector4{0.740f, 0.740f, 0.740f, 0.350f};
        static constexpr auto TransGray         = Vector4{0.410f, 0.410f, 0.410f, 0.500f};
        static constexpr auto TransGrayAlt      = Vector4{0.610f, 0.610f, 0.610f, 0.390f};
        static constexpr auto TransGrayDark     = Vector4{0.017f, 0.017f, 0.017f, 0.530f};
        
        static constexpr auto AccentDark        = Vector4{0.000f, 0.447f, 0.000f, 1.000f};
        static constexpr auto Accent            = Vector4{0.000f, 0.490f, 0.000f, 1.000f};
        static constexpr auto AccentLight       = Vector4{0.000f, 0.729f, 0.000f, 1.000f};
        static constexpr auto AccentTrans       = Vector4{0.000f, 0.990f, 0.000f, 0.050f};
    };
}