/**
 * @file Tag.h
 * @copyright Jaremie Romer and McCallister Romer 2024
 */
#pragma once

#include "Component.h"

#include <string>
#include <string_view>

namespace nc
{
/**
 * @brief Component with a string tag.
 * 
 * @note Tags are automatically added and removed with their parent
 *       Entity. They do not need to be explicity created.
 * 
 * A Tag's initial value can be set in EntityInfo when adding an Entity.
 */
struct Tag
{
    std::string value;
};
} // namespace nc
