#include "ncengine/graphics/SkeletalAnimationTypes.h"
#include "ncasset/Assets.h"

#include <ranges>

namespace
{
auto GetGlobalInverseTransform(const std::vector<nc::asset::BoneSpaceToParentSpace>& offsets) -> DirectX::XMMATRIX
{
    static constexpr auto root = std::string_view{"Root"};
    const auto pos = std::ranges::find_if(offsets, [](const auto& offset)
    {
        return offset.boneName.find(root) != std::string::npos;
    });
    return pos == offsets.end() ? DirectX::XMMatrixIdentity() : DirectX::XMMatrixInverse(nullptr, pos->transformationMatrix);
}
}

namespace nc::graphics::anim
{
PackedRig::PackedRig(const nc::asset::BonesData& bonesData)
    : vertexToBone{},
      boneToParent{},
      globalInverseTransform{GetGlobalInverseTransform(bonesData.boneSpaceToParentSpace)},
      boneNames{},
      boneToParentIndices{},
      offsetsMap{}
{
    auto& bspsVec = bonesData.boneSpaceToParentSpace;
    auto& vsbsVec = bonesData.vertexSpaceToBoneSpace;

    vertexToBone.reserve(vsbsVec.size());
    boneToParent.reserve(bspsVec.size());
    boneNames.reserve(bspsVec.size());
    boneToParentIndices.reserve(bspsVec.size());
    offsetsMap.reserve(vsbsVec.size());

    std::ranges::transform(vsbsVec, std::back_inserter(vertexToBone),
        [](const nc::asset::VertexSpaceToBoneSpace& vsbs) -> DirectX::XMMATRIX { return vsbs.transformationMatrix; });

    std::ranges::transform(bspsVec, std::back_inserter(boneToParent),
        [](const nc::asset::BoneSpaceToParentSpace& bsps) -> DirectX::XMMATRIX { return bsps.transformationMatrix; });

    std::ranges::transform(bspsVec, std::back_inserter(boneNames),
        [](const nc::asset::BoneSpaceToParentSpace& bsps) -> std::string { return bsps.boneName; });

    std::ranges::transform(bspsVec, std::back_inserter(boneToParentIndices),
        [](const nc::asset::BoneSpaceToParentSpace& bsps) -> std::pair<uint32_t, uint32_t> { return std::make_pair(bsps.indexOfFirstChild, bsps.numChildren); });
    
    // Create a vector of uint32_t that represent the vertexOffset's corresponding bone index in the boneSpace offset vector.
    // This is important because we need to keep the two vectors in sync but the boneSpace vector contains data not present or need in the vertexOffset vector.
    std::ranges::for_each(bonesData.vertexSpaceToBoneSpace, [&map = offsetsMap, &localBsPs = bonesData.boneSpaceToParentSpace](auto&& node)
    {
        auto pos = std::ranges::find(localBsPs, node.boneName, [](auto&& bsPsNode){ return bsPsNode.boneName; });
        if (pos != localBsPs.end())
        {
            map.push_back(static_cast<uint32_t>(std::distance(localBsPs.begin(), pos)));
        }
    });
}
}