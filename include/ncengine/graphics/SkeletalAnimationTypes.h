#pragma once

#include "ncengine/ecs/Component.h"
#include "ncasset/Assets.h"

namespace nc::graphics::anim
{
enum class Action : uint8_t
{
    Loop
};

struct PlayState
{
    std::string meshUid;
    std::string prevAnimUid;
    std::string curAnimUid;
    Action action;
    Entity entity;
};

struct Initial
{
    Initial(std::string animUid_);
    std::string animUid;
};

struct State
{
    static constexpr uint32_t NullId = UINT32_MAX;
    State(const Initial& initialProperties);

    uint32_t id;
    Action action;
    std::string animUid;
};

struct PackedRig
{
    PackedRig(nc::asset::BonesData bonesData);

    std::vector<DirectX::XMMATRIX> vertexToBone;
    std::vector<DirectX::XMMATRIX> boneToParent;
    DirectX::XMMATRIX globalInverseTransform;
    std::vector<std::string> boneNames;
    std::vector<std::tuple<uint32_t, uint32_t>> boneToParentIndices;
    std::vector<uint32_t> offsetsMap;
};

struct DecomposedMatrix
{
    nc::Vector3 pos;
    nc::Quaternion rot;
    nc::Vector3 scale;
};

using HasValue = uint8_t;
struct PackedAnimation
{
    std::vector<DirectX::XMMATRIX> offsets;
    std::vector<HasValue> hasValues;
};

struct PackedAnimationDecomposed
{
    std::vector<DecomposedMatrix> offsets;
    std::vector<HasValue> hasValues;
};

struct UnitOfWork
{
    Entity::index_type index;
    PackedRig* rig;
    asset::SkeletalAnimation* blendFromAnim;
    asset::SkeletalAnimation* blendToAnim;
    float blendFromTime;
    float blendToTime;
    float blendFactor;
};
} // namespace nc::graphics::anim
