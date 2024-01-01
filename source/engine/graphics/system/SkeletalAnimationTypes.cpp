#include "SkeletalAnimationTypes.h"

#include "ncasset/Assets.h"

namespace
{
auto GetGlobalInverseTransform(const std::vector<nc::asset::BoneSpaceToParentSpace>& offsets) -> DirectX::XMMATRIX
{
    const auto pos = std::ranges::find_if(offsets, [](const auto& offset)
    {
        return offset.boneName.find(std::string{"Root"}) != std::string::npos;
    });
    return pos == offsets.end() ? DirectX::XMMatrixIdentity() : DirectX::XMMatrixInverse(nullptr, pos->transformationMatrix);
}
}

namespace nc::graphics::anim
{
Initial::Initial(std::string animUid_)
    : animUid{std::move(animUid_)}{}

State::State(const Initial& initialProperties)
    : id{},
      action{Action::Loop},
      animUid{std::move(initialProperties.animUid)}{}

PackedRig::PackedRig(nc::asset::BonesData bonesData)
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

    std::transform(vsbsVec.begin(), vsbsVec.end(), std::back_inserter(vertexToBone),
                [](const nc::asset::VertexSpaceToBoneSpace& vsbs) -> DirectX::XMMATRIX { return vsbs.transformationMatrix; });

    std::transform(bspsVec.begin(), bspsVec.end(), std::back_inserter(boneToParent),
                [](const nc::asset::BoneSpaceToParentSpace& bsps) -> DirectX::XMMATRIX { return bsps.transformationMatrix; });

    std::transform(bspsVec.begin(), bspsVec.end(), std::back_inserter(boneNames),
                   [](const nc::asset::BoneSpaceToParentSpace& bsps) -> std::string { return bsps.boneName; });

    std::transform(bspsVec.begin(), bspsVec.end(), std::back_inserter(boneToParentIndices),
                [](const nc::asset::BoneSpaceToParentSpace& bsps) -> std::tuple<uint32_t, uint32_t> { return std::make_tuple(bsps.indexOfFirstChild, bsps.numChildren); });

    for (const auto& vsbs : bonesData.vertexSpaceToBoneSpace)
    {
        auto pos = std::ranges::find_if(bonesData.boneSpaceToParentSpace, [vsbs](const auto& bsps)
        {
            return vsbs.boneName == bsps.boneName;
        });

        if (pos != bonesData.boneSpaceToParentSpace.end())
        {
            offsetsMap.push_back(static_cast<uint32_t>(std::distance(bonesData.boneSpaceToParentSpace.begin(), pos)));
        }
    }
}
}