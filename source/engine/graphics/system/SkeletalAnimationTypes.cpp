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

SparseSet::SparseSet(uint32_t maxStates)
    : m_dense{},
      m_sparse(maxStates, State::NullId)
{
    m_dense.reserve(maxStates);
}

auto SparseSet::Insert(State toInsert) -> uint32_t
{
    toInsert.id = AssignId();
    if (toInsert.id >= m_sparse.size())
    {
        throw nc::NcError("Capacity reached. Cannot insert state.");
    }

    m_sparse.at(toInsert.id) = static_cast<uint32_t>(m_dense.size());
    m_dense.push_back(std::move(toInsert));
    return m_dense.back().id;
}

auto SparseSet::Remove(uint32_t toRemove) -> bool
{
    if (!Contains(toRemove))
        return false;

    const auto toRemoveSparse = toRemove;
    const auto toRemoveDense = m_sparse.at(toRemoveSparse);

    m_dense.at(toRemoveDense) = std::move(m_dense.back());
    m_dense.pop_back();
    m_sparse.at(toRemoveSparse) = State::NullId;
    m_sparse.at(m_dense.at(toRemoveDense).id) = toRemoveDense;
    return true;
}

auto SparseSet::Contains(uint32_t id) -> bool
{
    return id < m_sparse.size() ? m_sparse[id] != State::NullId : false;
}

auto SparseSet::Get(uint32_t id) -> State*
{
    return &m_dense.at(m_sparse.at(id));
}

auto SparseSet::GetLast() -> State*
{
    return &m_dense.back();
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