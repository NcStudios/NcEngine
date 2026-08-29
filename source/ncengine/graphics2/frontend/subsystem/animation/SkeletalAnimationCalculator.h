#pragma once

#include "Rig.h"
#include "graphics2/ShaderTypes.h"

#include "ncengine/ecs/Component.h"
#include "ncasset/Assets.h"
#include "ncmath/MatrixUtilities.h"

#include <limits>

namespace nc::graphics
{
class SkeletalAnimationCalculator
{
    public:
        auto Animate(uint64_t meshId,
                     const Rig& rig,
                     const asset::SkeletalAnimation& animation,
                     float timeInTicks,
                     std::vector<std::string>& boneNamesOut) -> std::span<const BoneData>;

        auto Animate(uint64_t meshId,
                     const Rig& rig,
                     const asset::SkeletalAnimation& blendFromAnimation,
                     float blendFromTicks,
                     const asset::SkeletalAnimation& blendToAnimation,
                     float blendToTicks,
                     float blendFactor,
                     std::vector<std::string>& boneNamesOut) -> std::span<const BoneData>;

    private:
        std::vector<BoneData> m_boneBuffer;
        std::vector<DirectX::XMMATRIX> m_offsets;
        std::vector<DecomposedMatrixXM> m_fromOffsetsDecomposed;
        std::vector<DecomposedMatrixXM> m_toOffsetsDecomposed;

        void Prepare(const Rig& rig, bool blended);
};
} // namespace nc::graphics
