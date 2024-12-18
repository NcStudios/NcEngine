#pragma once

#include "ncasset/Assets.h"

#include "DirectXMath.h"

#include <limits>
#include <vector>

namespace nc::graphics
{
// Used to traverse the flattened offsets tree.
struct OffsetChildren
{
    uint32_t indexOfFirstChild;
    uint32_t numChildren;
};

// An SoA representation of nc::asset::BonesData.
struct Rig
{
    explicit Rig(const nc::asset::BonesData& bonesData);

    std::vector<DirectX::XMMATRIX> vertexToBone;
    std::vector<DirectX::XMMATRIX> boneToParent;
    DirectX::XMMATRIX globalInverseTransform;
    std::vector<std::string> boneNames;
    std::vector<OffsetChildren> offsetChildren;
    std::vector<uint32_t> offsetsMap;
};
} // namespace nc::graphics
