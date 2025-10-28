/**
 * @file Environment.h
 * @copyright Jaremie Romer and McCallister Romer 2025
 */
#pragma once

#include "ncengine/asset/AssetViews.h"
#include "ncmath/Vector.h"

namespace nc
{
/** @brief Contains global graphics properties. */
struct Environment
{
    Vector4 primaryColor = Vector4::One();
    Vector4 secondaryColor = Vector4::One();
    Vector4 tertiaryColor = Vector4::One();
    asset::CubeMapView skybox = asset::CubeMapView{};
    bool useSkybox = false;
    bool useColorOverride = false;
};
} // namespace nc