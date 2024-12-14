#pragma once

#include "SkeletalAnimationTypes.h"
#include "graphics2/ShaderTypes.h"

#include "ncengine/ecs/Component.h"
#include "ncasset/Assets.h"
#include "ncmath/MatrixUtilities.h"

#include <limits>

namespace nc::graphics::gfx3
{

// could keep vectors in a context block

class SkeletalAnimationCalculator
{
    public:
        auto Animate(const gfx2::PackedRig& rig,
                     const asset::SkeletalAnimation& animation,
                     float timeInTicks) -> std::span<const BoneData>;

        auto Animate(const gfx2::PackedRig& rig,
                     const asset::SkeletalAnimation& blendFromAnimation,
                     float blendFromTicks,
                     const asset::SkeletalAnimation& blendToAnimation,
                     float blendToTicks,
                     float blendFactor) -> std::span<const BoneData>;

    private:
        std::vector<BoneData> m_boneBuffer;
        std::vector<DirectX::XMMATRIX> m_offsets;
        std::vector<DecomposedMatrixXM> m_fromOffsetsDecomposed;
        std::vector<DecomposedMatrixXM> m_toOffsetsDecomposed;
        std::vector<uint8_t> m_hasValues;

        void Prepare(size_t boneCapacity);
};
} // namespace nc::graphics::gfx2
