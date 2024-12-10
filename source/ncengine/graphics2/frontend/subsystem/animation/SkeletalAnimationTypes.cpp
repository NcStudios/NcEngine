#include "SkeletalAnimationTypes.h"

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

    return pos == offsets.end()
        ? DirectX::XMMatrixIdentity()
        : DirectX::XMMatrixInverse(nullptr, pos->transformationMatrix);
}
} // anonymous namespace

namespace nc::graphics::gfx2
{
PackedRig::PackedRig(const asset::BonesData& bonesData)
    : vertexToBone{},
      boneToParent{},
      globalInverseTransform{GetGlobalInverseTransform(bonesData.boneSpaceToParentSpace)},
      boneNames{},
      offsetChildren{},
      offsetsMap{}
{
    auto& bspsVec = bonesData.boneSpaceToParentSpace;
    auto& vsbsVec = bonesData.vertexSpaceToBoneSpace;

    vertexToBone.reserve(vsbsVec.size());
    boneToParent.reserve(bspsVec.size());
    boneNames.reserve(bspsVec.size());
    offsetChildren.reserve(bspsVec.size());
    offsetsMap.reserve(vsbsVec.size());

    std::ranges::transform(vsbsVec, std::back_inserter(vertexToBone), &asset::VertexSpaceToBoneSpace::transformationMatrix);
    std::ranges::transform(bspsVec, std::back_inserter(boneToParent), &asset::BoneSpaceToParentSpace::transformationMatrix);
    std::ranges::transform(bspsVec, std::back_inserter(boneNames), &asset::BoneSpaceToParentSpace::boneName);
    std::ranges::transform(
        bspsVec,
        std::back_inserter(offsetChildren),
        [](const auto& bsps) {
            return OffsetChildren{
                .indexOfFirstChild = bsps.indexOfFirstChild,
                .numChildren = bsps.numChildren
            };
        }
    );

    // Create a vector of uint32_t that represent the vertexOffset's corresponding bone index in the boneSpace offset vector.
    // This is important because we need to keep the two vectors in sync but the boneSpace vector contains data not present or need in the vertexOffset vector.
    std::ranges::for_each(bonesData.vertexSpaceToBoneSpace, [&map = offsetsMap, &localBsPs = bonesData.boneSpaceToParentSpace](auto&& node)
    {
        auto pos = std::ranges::find(localBsPs, node.boneName, &asset::BoneSpaceToParentSpace::boneName);
        if (pos != localBsPs.end())
        {
            map.push_back(static_cast<uint32_t>(std::distance(localBsPs.begin(), pos)));
        }
    });
}
} // namespace nc::anim
