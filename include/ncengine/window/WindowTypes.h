/**
 * @file WindowTypes.h
 * @copyright Jaremie Romer and McCallister Romer 2025
 */
#pragma once

#include "ncmath/Vector.h"

namespace nc
{
/** @brief A struct that controls the viewport and scissor size and placement. 
 * @remarks Viewport vectors are scalars between 0.0 and 1.0f representing ratio of screen size.
 * @remarks Only supports a single viewport now.
*/
struct Viewport
{
    nc::Vector2 Size    = nc::Vector2{1.0f, 1.0f};
    nc::Vector2 TopLeft = nc::Vector2{0.0f, 0.0f};
};
}