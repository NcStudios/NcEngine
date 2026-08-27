/**
 * @file BoneSnapper.h
 * @copyright Jaremie Romer and McCallister Romer 2026
 */

 #pragma once

 #include "ncengine/ecs/Entity.h"

#include <string>

 namespace nc
 {
    struct BoneSnapper
    {
        std::string boneName;
        Entity target;
    };
 } // namespace nc