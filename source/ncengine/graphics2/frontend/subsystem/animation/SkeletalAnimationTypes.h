#pragma once

#include "ncengine/ecs/Component.h"
#include "ncasset/Assets.h"

#include <limits>

namespace nc::graphics::gfx2
{
/**
 * @brief Used to traverse the flattened offsets tree.
 */
struct OffsetChildren
{
    uint32_t indexOfFirstChild;
    uint32_t numChildren;
};

/**
 * @brief An SoA representation of nc::asset::BonesData.
 * 
 * SkeletalAnimationSystem uses this type to efficiently perform animation transformation calculations.
 */
struct PackedRig
{
    PackedRig(const nc::asset::BonesData& bonesData);

    std::vector<DirectX::XMMATRIX> vertexToBone;
    std::vector<DirectX::XMMATRIX> boneToParent;
    DirectX::XMMATRIX globalInverseTransform;
    std::vector<std::string> boneNames;
    std::vector<OffsetChildren> offsetChildren;
    std::vector<uint32_t> offsetsMap;
};
} // namespace nc::anim
