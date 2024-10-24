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
void State::AddSuccessor(uint32_t successor)
{
    successors.push_back(successor);
}

StableSet::StableSet()
    : m_states{}
{
}

auto StableSet::Insert(State toInsert) -> uint32_t
{
    toInsert.id = AssignId();
    m_states.push_back(std::move(toInsert));
    return m_states.back().id;
}

auto StableSet::Remove(uint32_t toRemove) noexcept -> bool
{
    if (!Contains(toRemove))
        return false;

    m_states.at(toRemove).id = NullState;
    return true;
}

auto StableSet::Contains(uint32_t id) const noexcept -> bool
{
    return id < m_states.size() ? m_states[id].id != NullState : false;
}

auto StableSet::Get(uint32_t id) -> State&
{
    auto& state = m_states.at(id);
    NC_ASSERT(state.id != NullState, "The last added state was set to null.");
    return state;
}

auto StableSet::GetLast() -> State&
{
    NC_ASSERT(!m_states.empty(), "The states container is empty.");
    auto& back = m_states.back();
    NC_ASSERT(back.id != NullState, "The last added state was set to null."); 
    return back;
}

PackedRig::PackedRig(const nc::asset::BonesData& bonesData)
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

    std::ranges::transform(vsbsVec, std::back_inserter(vertexToBone),
        [](const nc::asset::VertexSpaceToBoneSpace& vsbs) -> DirectX::XMMATRIX { return vsbs.transformationMatrix; });

    std::ranges::transform(bspsVec, std::back_inserter(boneToParent),
        [](const nc::asset::BoneSpaceToParentSpace& bsps) -> DirectX::XMMATRIX { return bsps.transformationMatrix; });

    std::ranges::transform(bspsVec, std::back_inserter(boneNames),
        [](const nc::asset::BoneSpaceToParentSpace& bsps) -> std::string { return bsps.boneName; });

    std::ranges::transform(bspsVec, std::back_inserter(offsetChildren),
        [](const nc::asset::BoneSpaceToParentSpace& bsps) -> OffsetChildren { return OffsetChildren { .indexOfFirstChild = bsps.indexOfFirstChild, .numChildren = bsps.numChildren }; });
    
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