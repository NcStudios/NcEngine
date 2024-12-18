#include "gtest/gtest.h"
#include "graphics2/frontend/subsystem/animation/SkeletalAnimationCalculator.h"

#include <ranges>

auto MakeAnimation() -> nc::asset::SkeletalAnimation
{
    using namespace nc;
    using namespace nc::asset;
    auto positionFrame = PositionFrame{0.0f, Vector3{1.0f, 1.0f, 1.0f}};
    auto positionFrame2 = PositionFrame{1.0f, Vector3{2.0f, 2.0f, 2.0f}};
    auto positionFrame3 = PositionFrame{2.0f, Vector3{3.0f, 3.0f, 3.0f}};
    auto positionFrames = std::vector<PositionFrame>{positionFrame, positionFrame2, positionFrame3};

    auto rotationFrame = RotationFrame{0.0f, Quaternion::Identity()};
    auto rotationFrame2 = RotationFrame{1.0f, Quaternion::Identity()};
    auto rotationFrame3 = RotationFrame{2.0f, Quaternion::Identity()};
    auto rotationFrames = std::vector<RotationFrame>{rotationFrame, rotationFrame2, rotationFrame3};

    auto scaleFrame = ScaleFrame{0.0f, Vector3{1.0f, 1.0f, 1.0f}};
    auto scaleFrame2 = ScaleFrame{1.0f, Vector3{2.0f, 2.0f, 2.0f}};
    auto scaleFrame3 = ScaleFrame{2.0f, Vector3{3.0f, 3.0f, 3.0f}};
    auto scaleFrames = std::vector<ScaleFrame>{scaleFrame, scaleFrame2, scaleFrame3};

    auto frames = SkeletalAnimationFrames{
        .positionFrames = std::move(positionFrames),
        .rotationFrames = std::move(rotationFrames),
        .scaleFrames = std::move(scaleFrames),
    };

    auto framesPerBones = std::unordered_map<std::string, SkeletalAnimationFrames>{};
    framesPerBones.emplace("Bone0", frames);
    framesPerBones.emplace("Bone1", frames);
    framesPerBones.emplace("Bone3", SkeletalAnimationFrames{});

    return SkeletalAnimation{
        .name = "test",
        .durationInTicks = 1,
        .ticksPerSecond = 1,
        .framesPerBone = std::move(framesPerBones)
    };
}

auto MakeBonesData() -> nc::asset::BonesData
{
    using namespace nc::asset;
    auto bonesData = BonesData{
        .boneMapping = std::unordered_map<std::string, uint32_t>{},
        .vertexSpaceToBoneSpace = std::vector<VertexSpaceToBoneSpace>(0),
        .boneSpaceToParentSpace = std::vector<BoneSpaceToParentSpace>(0)
    };

    const auto matrix1 = DirectX::XMMATRIX{
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1
    };

    const auto matrix2 = DirectX::XMMATRIX{
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        1, 0, 0, 1
    };

    bonesData.vertexSpaceToBoneSpace.emplace_back(std::string("Bone0"), matrix1);
    bonesData.vertexSpaceToBoneSpace.emplace_back(std::string("Bone1"), matrix1);
    bonesData.boneSpaceToParentSpace.emplace_back(std::string("Bone0"), matrix2, 0u, 0u);
    bonesData.boneSpaceToParentSpace.emplace_back(std::string("Bone1"), matrix2, 0u, 0u);
    bonesData.boneSpaceToParentSpace.emplace_back(std::string("Bone2"), matrix2, 0u, 0u);
    bonesData.boneMapping.emplace("Bone0", 0);
    bonesData.boneMapping.emplace("Bone1", 1);
    bonesData.boneMapping.emplace("Bone2", 2);
    return bonesData;
}

const auto g_animation = MakeAnimation();
const auto g_bonesData = MakeBonesData();

// at t=0, animation scales and translates by 1
const auto expectedMatrixT0 = DirectX::XMMatrixSet(
    1, 0, 0, 0,
    0, 1, 0, 0,
    0, 0, 1, 0,
    1, 1, 1, 1
);

// at t=0.5, animation interpolates between t=0 and t=1
const auto expectedMatrixT0_5 = DirectX::XMMatrixSet(
    1.5,   0,   0, 0,
        0, 1.5,   0, 0,
        0,   0, 1.5, 0,
    1.5, 1.5, 1.5, 1
);

// at t=1, animation scales and translates by 2
const auto expectedMatrixT1 = DirectX::XMMatrixSet(
    2, 0, 0, 0,
    0, 2, 0, 0,
    0, 0, 2, 0,
    2, 2, 2, 1
);

auto MatrixEqual(DirectX::FXMMATRIX lhs, DirectX::FXMMATRIX rhs) -> bool
{
    return DirectX::XMVector4Equal(lhs.r[0], rhs.r[0]) &&
           DirectX::XMVector4Equal(lhs.r[1], rhs.r[1]) &&
           DirectX::XMVector4Equal(lhs.r[2], rhs.r[2]) &&
           DirectX::XMVector4Equal(lhs.r[3], rhs.r[3]);
}

TEST(SkeletalAnimationCalculatorTest, Rig_constructedFromBonesData)
{
    const auto uut = nc::graphics::Rig{g_bonesData};
    EXPECT_TRUE(std::ranges::equal(
        g_bonesData.vertexSpaceToBoneSpace,
        uut.vertexToBone,
        &MatrixEqual,
        &nc::asset::VertexSpaceToBoneSpace::transformationMatrix
    ));

    EXPECT_TRUE(std::ranges::equal(
        g_bonesData.boneSpaceToParentSpace,
        uut.boneToParent,
        &MatrixEqual,
        &nc::asset::BoneSpaceToParentSpace::transformationMatrix
    ));

    EXPECT_TRUE(MatrixEqual(DirectX::XMMatrixIdentity(), uut.globalInverseTransform));

    EXPECT_TRUE(std::ranges::equal(
        g_bonesData.boneSpaceToParentSpace,
        uut.boneNames,
        std::equal_to{},
        &nc::asset::BoneSpaceToParentSpace::boneName
    ));

    EXPECT_TRUE(std::ranges::equal(
        g_bonesData.boneSpaceToParentSpace,
        uut.offsetChildren,
        [](const auto& expected, const auto& actual){
            return expected.indexOfFirstChild == actual.indexOfFirstChild &&
                   expected.numChildren == actual.numChildren;
        }
    ));
}

TEST(SkeletalAnimationCalculatorTest, Animate_single_succeeds)
{
    const auto rig = nc::graphics::Rig{g_bonesData};
    auto uut = nc::graphics::SkeletalAnimationCalculator{};

    auto actual = uut.Animate(rig, g_animation, 0.0f);
    ASSERT_EQ(rig.vertexToBone.size(), actual.size());
    EXPECT_TRUE(MatrixEqual(expectedMatrixT0, actual[0].animatedBoneMatrix));
    EXPECT_TRUE(MatrixEqual(expectedMatrixT0, actual[1].animatedBoneMatrix));

    actual = uut.Animate(rig, g_animation, 0.5f);
    ASSERT_EQ(rig.vertexToBone.size(), actual.size());
    EXPECT_TRUE(MatrixEqual(expectedMatrixT0_5, actual[0].animatedBoneMatrix));
    EXPECT_TRUE(MatrixEqual(expectedMatrixT0_5, actual[1].animatedBoneMatrix));

    actual = uut.Animate(rig, g_animation, 1.0f);
    ASSERT_EQ(rig.vertexToBone.size(), actual.size());
    EXPECT_TRUE(MatrixEqual(expectedMatrixT1, actual[0].animatedBoneMatrix));
    EXPECT_TRUE(MatrixEqual(expectedMatrixT1, actual[1].animatedBoneMatrix));
}

TEST(SkeletalAnimationCalculatorTest, Animate_blended_succeeds)
{
    const auto rig = nc::graphics::Rig{g_bonesData};
    auto uut = nc::graphics::SkeletalAnimationCalculator{};

    // Since we're blending between the same animation, this allows using blend factors 0/0.5/1 to get the same
    // interpolated results as the above test (e.g. if blend from uses frames at t=0, and blend at t=1, then we
    // expect a blend factor of 0.5 to be equivalent to the interpolated frame at t=0.5).
    constexpr auto blendFromTicks = 0.0f;
    constexpr auto blendToTicks = 1.0f;

    auto blendFactor = 0.0f;
    auto actual = uut.Animate(rig, g_animation, blendFromTicks, g_animation, blendToTicks, blendFactor);
    ASSERT_EQ(rig.vertexToBone.size(), actual.size());
    EXPECT_TRUE(MatrixEqual(expectedMatrixT0, actual[0].animatedBoneMatrix));
    EXPECT_TRUE(MatrixEqual(expectedMatrixT0, actual[1].animatedBoneMatrix));

    blendFactor = 0.5f;
    actual = uut.Animate(rig, g_animation, blendFromTicks, g_animation, blendToTicks, blendFactor);
    ASSERT_EQ(rig.vertexToBone.size(), actual.size());
    EXPECT_TRUE(MatrixEqual(expectedMatrixT0_5, actual[0].animatedBoneMatrix));
    EXPECT_TRUE(MatrixEqual(expectedMatrixT0_5, actual[1].animatedBoneMatrix));

    blendFactor = 1.0f;
    actual = uut.Animate(rig, g_animation, blendFromTicks, g_animation, blendToTicks, blendFactor);
    ASSERT_EQ(rig.vertexToBone.size(), actual.size());
    EXPECT_TRUE(MatrixEqual(expectedMatrixT1, actual[0].animatedBoneMatrix));
    EXPECT_TRUE(MatrixEqual(expectedMatrixT1, actual[1].animatedBoneMatrix));
}
