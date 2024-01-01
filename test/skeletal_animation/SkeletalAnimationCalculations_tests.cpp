#include "gtest/gtest.h"
#include "asset/AssetData.h"
#include "graphics/system/SkeletalAnimationCalculations.h"

#include <exception>

using namespace nc;

struct TestData
{
    nc::asset::BoneSpaceToParentSpace boneOffset;
    nc::asset::SkeletalAnimation animation;
    nc::asset::BonesData bonesData;
    std::vector<std::string> boneNames;
};

class SkeletalAnimationCalculations_tests : public ::testing::Test
{
    public:
        std::unique_ptr<TestData> testData;

    protected:
        void SetUp() override
        {
            auto positionFrame = nc::asset::PositionFrame
            {
                .timeInTicks = 0.0f,
                .position = nc::Vector3{1.0f, 1.0f, 1.0f}
            };

            auto positionFrame2 = nc::asset::PositionFrame
            {
                .timeInTicks = 1.0f,
                .position = nc::Vector3{2.0f, 2.0f, 2.0f}
            };

            auto positionFrame3 = nc::asset::PositionFrame
            {
                .timeInTicks = 2.0f,
                .position = nc::Vector3{3.0f, 3.0f, 3.0f}
            };

            auto positionFrames = std::vector<nc::asset::PositionFrame>
            {
                positionFrame,
                positionFrame2,
                positionFrame3
            };

            auto rotationFrame = nc::asset::RotationFrame
            {
                .timeInTicks = 0.0f,
                .rotation = nc::Quaternion::Identity()
            };

            auto rotationFrame2 = nc::asset::RotationFrame
            {
                .timeInTicks = 1.0f,
                .rotation = nc::Quaternion::Identity()
            };

            auto rotationFrame3 = nc::asset::RotationFrame
            {
                .timeInTicks = 2.0f,
                .rotation = nc::Quaternion::Identity()
            };

            auto rotationFrames = std::vector<nc::asset::RotationFrame>
            {
                rotationFrame,
                rotationFrame2,
                rotationFrame3
            };

            auto scaleFrame = nc::asset::ScaleFrame
            {
                .timeInTicks = 0.0f,
                .scale = nc::Vector3{1.0f, 1.0f, 1.0f}
            };

            auto scaleFrame2 = nc::asset::ScaleFrame
            {
                .timeInTicks = 1.0f,
                .scale = nc::Vector3{2.0f, 2.0f, 2.0f}
            };

            auto scaleFrame3 = nc::asset::ScaleFrame
            {
                .timeInTicks = 2.0f,
                .scale = nc::Vector3{3.0f, 3.0f, 3.0f}
            };

            auto scaleFrames = std::vector<nc::asset::ScaleFrame>
            {
                scaleFrame,
                scaleFrame2,
                scaleFrame3
            }; 
            
            auto frames = nc::asset::SkeletalAnimationFrames
            {
                .positionFrames = std::move(positionFrames),
                .rotationFrames = std::move(rotationFrames),
                .scaleFrames = std::move(scaleFrames),
            };

            auto framesPerBones = std::unordered_map<std::string, nc::asset::SkeletalAnimationFrames>{};
            framesPerBones.emplace("Bone0", frames);
            framesPerBones.emplace("Bone1", frames);
            framesPerBones.emplace("Bone3", nc::asset::SkeletalAnimationFrames{});

            auto animation = nc::asset::SkeletalAnimation
            {
                .name = "test",
                .durationInTicks = 1,
                .ticksPerSecond = 1,
                .framesPerBone = std::move(framesPerBones)
            };

            auto boneOffset = nc::asset::BoneSpaceToParentSpace
            {
                .boneName = "Bone0",
                .transformationMatrix = DirectX::XMMatrixIdentity(),
                .numChildren = 0,
                .indexOfFirstChild = 0
            };

            auto mesh = nc::asset::Mesh{
                .extents = nc::Vector3{-5.0f, 4.22f, 10.010101f},
                .maxExtent = 10.010101f,
                .vertices = std::vector<nc::asset::MeshVertex>{
                    nc::asset::MeshVertex{nc::Vector3::Splat(0.0f),
                                        nc::Vector3::Splat(1.0f),
                                        nc::Vector2::Splat(2.0f),
                                        nc::Vector3::Splat(3.0f),
                                        nc::Vector3::Splat(4.0f),
                                        nc::Vector4::Splat(5.0f),
                                        std::array<uint32_t, 4>{6, 6, 6, 6}},
                    nc::asset::MeshVertex{nc::Vector3::Splat(7.0f),
                                        nc::Vector3::Splat(8.0f),
                                        nc::Vector2::Splat(9.0f),
                                        nc::Vector3::Splat(10.0f),
                                        nc::Vector3::Splat(11.0f),
                                        nc::Vector4::Splat(12.0f),
                                        std::array<uint32_t, 4>{13, 13, 13, 13}},
                    nc::asset::MeshVertex{nc::Vector3::Splat(14.0f),
                                        nc::Vector3::Splat(15.0f),
                                        nc::Vector2::Splat(16.0f),
                                        nc::Vector3::Splat(17.0f),
                                        nc::Vector3::Splat(18.0f),
                                        nc::Vector4::Splat(19.0f),
                                        std::array<uint32_t, 4>{20, 20, 20, 20}}
                },
                .indices = std::vector<uint32_t>{
                    0, 1, 2,  1, 2, 0,  2, 0, 1
                },
                .bonesData = nc::asset::BonesData{
                    .boneMapping = std::unordered_map<std::string, uint32_t>{},
                    .vertexSpaceToBoneSpace = std::vector<nc::asset::VertexSpaceToBoneSpace>(0),
                    .boneSpaceToParentSpace = std::vector<nc::asset::BoneSpaceToParentSpace>(0)
                }
            };

            // Can't initialize above due to internal compiler error in MS.
            mesh.bonesData.value().vertexSpaceToBoneSpace.push_back(
                nc::asset::VertexSpaceToBoneSpace
                {
                    .boneName = std::string("Bone0"),
                    .transformationMatrix = DirectX::XMMATRIX
                    {
                        -1, -1, -1, -1,
                        0, 0, 0, 0,
                        0, 0, 0, 0,
                        0, 0, 0, 0
                    }
                }
            );

            mesh.bonesData.value().vertexSpaceToBoneSpace.push_back(
                nc::asset::VertexSpaceToBoneSpace
                {
                    .boneName = std::string("Bone1"),
                    .transformationMatrix = DirectX::XMMATRIX
                    {
                        -1, -1, -1, -1,
                        0, 0, 0, 0,
                        0, 0, 0, 0,
                        0, 0, 0, 0
                    }
                }
            );

            // Can't initialize above due to internal compiler error in MS.
            mesh.bonesData.value().boneSpaceToParentSpace.push_back(
                nc::asset::BoneSpaceToParentSpace
                {
                    .boneName = std::string("Bone0"),
                    .transformationMatrix = DirectX::XMMATRIX
                    {
                        0, 0, 0, 1,
                        0, 0, 1, 0,
                        0, 1, 0, 0,
                        1, 0, 0, 0
                    },
                    .numChildren = 0u,
                    .indexOfFirstChild = 0u
                }
            );

            mesh.bonesData.value().boneSpaceToParentSpace.push_back(
                nc::asset::BoneSpaceToParentSpace
                {
                    .boneName = std::string("Bone1"),
                    .transformationMatrix = DirectX::XMMATRIX
                    {
                        0, 0, 0, 1,
                        0, 0, 1, 0,
                        0, 1, 0, 0,
                        1, 0, 0, 0
                    },
                    .numChildren = 0u,
                    .indexOfFirstChild = 0u
                }
            );

            mesh.bonesData.value().boneSpaceToParentSpace.push_back(
                nc::asset::BoneSpaceToParentSpace
                {
                    .boneName = std::string("Bone2"),
                    .transformationMatrix = DirectX::XMMATRIX
                    {
                        0, 0, 0, 1,
                        0, 0, 1, 0,
                        0, 1, 0, 0,
                        1, 0, 0, 0
                    },
                    .numChildren = 0u,
                    .indexOfFirstChild = 0u
                }
            );

            // Can't initialize above due to internal compiler error in MS.
            mesh.bonesData.value().boneMapping.emplace("Bone0", 0);
            mesh.bonesData.value().boneMapping.emplace("Bone1", 1);
            mesh.bonesData.value().boneMapping.emplace("Bone2", 2);

            testData = std::make_unique<TestData>();
            testData->boneOffset = boneOffset;
            testData->animation = animation;
            testData->bonesData = mesh.bonesData.value();
            testData->boneNames = std::vector<std::string>{"Bone0", "Bone1", "Bone2"};
        }

        void TearDown() override
        {
            testData = nullptr;
        }
};

bool MatrixEquals(const DirectX::XMMATRIX& a, const DirectX::XMMATRIX& b)
{
    DirectX::XMFLOAT4X4 aView;
    XMStoreFloat4x4(&aView, a);
    float aa1 = aView._11;
    float aa2 = aView._12;
    float aa3 = aView._13;
    float aa4 = aView._14;
    float ab1 = aView._21;
    float ab2 = aView._22;
    float ab3 = aView._23;
    float ab4 = aView._24;
    float ac1 = aView._31;
    float ac2 = aView._32;
    float ac3 = aView._33;
    float ac4 = aView._34;
    float ad1 = aView._41;
    float ad2 = aView._42;
    float ad3 = aView._43;
    float ad4 = aView._44;

    DirectX::XMFLOAT4X4 bView;
    XMStoreFloat4x4(&bView, b);
    float ba1 = bView._11;
    float ba2 = bView._12;
    float ba3 = bView._13;
    float ba4 = bView._14;
    float bb1 = bView._21;
    float bb2 = bView._22;
    float bb3 = bView._23;
    float bb4 = bView._24;
    float bc1 = bView._31;
    float bc2 = bView._32;
    float bc3 = bView._33;
    float bc4 = bView._34;
    float bd1 = bView._41;
    float bd2 = bView._42;
    float bd3 = bView._43;
    float bd4 = bView._44;

    return 
    nc::FloatEqual(aa1, ba1) &&
    nc::FloatEqual(aa2, ba2) &&
    nc::FloatEqual(aa3, ba3) &&
    nc::FloatEqual(aa4, ba4) &&

    nc::FloatEqual(ab1, bb1) &&
    nc::FloatEqual(ab2, bb2) &&
    nc::FloatEqual(ab3, bb3) &&
    nc::FloatEqual(ab4, bb4) &&

    nc::FloatEqual(ac1, bc1) &&
    nc::FloatEqual(ac2, bc2) &&
    nc::FloatEqual(ac3, bc3) &&
    nc::FloatEqual(ac4, bc4) &&

    nc::FloatEqual(ad1, bd1) &&
    nc::FloatEqual(ad2, bd2) &&
    nc::FloatEqual(ad3, bd3) &&
    nc::FloatEqual(ad4, bd4);
};

TEST_F(SkeletalAnimationCalculations_tests, GetInterpolatedPosition_emptyVector_throws)
{
    auto positionFrames = std::vector<nc::asset::PositionFrame>{};
    EXPECT_THROW(graphics::GetInterpolatedPosition(0.0f, positionFrames), std::runtime_error);
}

TEST_F(SkeletalAnimationCalculations_tests, GetInterpolatedPosition_oneFrame_returnsFrame)
{
    auto positionFrame = nc::asset::PositionFrame
    {
        .timeInTicks = 0.0f,
        .position = nc::Vector3{1.0f, 2.0f, 3.0f}
    };

    auto positionFrames = std::vector<nc::asset::PositionFrame>
    {
        positionFrame
    };

    auto result = graphics::GetInterpolatedPosition(0.0f, positionFrames);
    EXPECT_FLOAT_EQ(result.x, 1.0f);
    EXPECT_FLOAT_EQ(result.y, 2.0f);
    EXPECT_FLOAT_EQ(result.z, 3.0f);
}

TEST_F(SkeletalAnimationCalculations_tests, GetInterpolatedPosition_twoFrames_interpolates)
{
    auto positionFrame = nc::asset::PositionFrame
    {
        .timeInTicks = 0.0f,
        .position = nc::Vector3{0.0f, 0.0f, 0.0f}
    };

    auto positionFrame2 = nc::asset::PositionFrame
    {
        .timeInTicks = 2.0f,
        .position = nc::Vector3{1.0f, 1.0f, 1.0f}
    };

    auto positionFrames = std::vector<nc::asset::PositionFrame>
    {
        positionFrame, 
        positionFrame2
    };

    auto result = graphics::GetInterpolatedPosition(1.0f, positionFrames);
    EXPECT_FLOAT_EQ(result.x, 0.5f);
    EXPECT_FLOAT_EQ(result.y, 0.5f);
    EXPECT_FLOAT_EQ(result.z, 0.5f);
}

TEST_F(SkeletalAnimationCalculations_tests, GetInterpolatedRotation_emptyVector_throws)
{
    auto rotationFrames = std::vector<nc::asset::RotationFrame>{};
    EXPECT_THROW(graphics::GetInterpolatedRotation(0.0f, rotationFrames), std::runtime_error);
}

TEST_F(SkeletalAnimationCalculations_tests, GetInterpolatedRotation_oneFrame_returnsFrame)
{
    auto rotationFrame = nc::asset::RotationFrame
    {
        .timeInTicks = 0.0f,
        .rotation = nc::Quaternion{1.0f, 1.0f, 1.0, 1.0f}
    };

    auto rotationFrames = std::vector<nc::asset::RotationFrame>
    {
        rotationFrame
    };

    auto result = graphics::GetInterpolatedRotation(0.0f, rotationFrames);
    EXPECT_FLOAT_EQ(result.x, 0.5f);
    EXPECT_FLOAT_EQ(result.y, 0.5f);
    EXPECT_FLOAT_EQ(result.z, 0.5f);
    EXPECT_FLOAT_EQ(result.w, 0.5f);
}

TEST_F(SkeletalAnimationCalculations_tests, GetInterpolatedRotation_twoFrames_interpolates)
{
    auto rotationFrame = nc::asset::RotationFrame
    {
        .timeInTicks = 0.0f,
        .rotation = nc::Quaternion{0.0f, 0.0f, 0.0f, 0.0f}
    };

    auto rotationFrame2 = nc::asset::RotationFrame
    {
        .timeInTicks = 2.0f,
        .rotation = nc::Quaternion{1.0f, 1.0f, 1.0f, 1.0f}
    };

    auto rotationFrames = std::vector<nc::asset::RotationFrame>
    {
        rotationFrame, 
        rotationFrame2
    };

    auto result = graphics::GetInterpolatedRotation(1.0f, rotationFrames);
    EXPECT_FLOAT_EQ(result.x, 0.5f);
    EXPECT_FLOAT_EQ(result.y, 0.5f);
    EXPECT_FLOAT_EQ(result.z, 0.5f);
    EXPECT_FLOAT_EQ(result.w, 0.5f);
}

TEST_F(SkeletalAnimationCalculations_tests, GetInterpolatedScale_emptyVector_throws)
{
    auto scaleFrames = std::vector<nc::asset::ScaleFrame>{};
    EXPECT_THROW(graphics::GetInterpolatedScale(0.0f, scaleFrames), std::runtime_error);
}

TEST_F(SkeletalAnimationCalculations_tests, GetInterpolatedScale_oneFrame_returnsFrame)
{
    auto scaleFrame = nc::asset::ScaleFrame
    {
        .timeInTicks = 0.0f,
        .scale = nc::Vector3{1.0f, 2.0f, 3.0f}
    };

    auto scaleFrames = std::vector<nc::asset::ScaleFrame>
    {
        scaleFrame
    };

    auto result = graphics::GetInterpolatedScale(0.0f, scaleFrames);
    EXPECT_FLOAT_EQ(result.x, 1.0f);
    EXPECT_FLOAT_EQ(result.y, 2.0f);
    EXPECT_FLOAT_EQ(result.z, 3.0f);
}

TEST_F(SkeletalAnimationCalculations_tests, GetInterpolatedScale_twoFrames_interpolates)
{
    auto scaleFrame = nc::asset::ScaleFrame
    {
        .timeInTicks = 0.0f,
        .scale = nc::Vector3{0.0f, 0.0f, 0.0f}
    };

    auto scaleFrame2 = nc::asset::ScaleFrame
    {
        .timeInTicks = 2.0f,
        .scale = nc::Vector3{1.0f, 1.0f, 1.0f}
    };

    auto scaleFrames = std::vector<nc::asset::ScaleFrame>
    {
        scaleFrame, 
        scaleFrame2
    };

    auto result = graphics::GetInterpolatedScale(1.0f, scaleFrames);
    EXPECT_FLOAT_EQ(result.x, 0.5f);
    EXPECT_FLOAT_EQ(result.y, 0.5f);
    EXPECT_FLOAT_EQ(result.z, 0.5f);
}

TEST_F(SkeletalAnimationCalculations_tests, GetAnimationOffsets_twoBonesHaveAnimData_bothHaveValues)
{
    auto decomposedMatrices = nc::graphics::GetAnimationOffsets(0.0f, testData->boneNames, &testData->animation);
    EXPECT_EQ(decomposedMatrices.hasValues[0], 1);
    EXPECT_EQ(decomposedMatrices.hasValues[1], 1);
}

TEST_F(SkeletalAnimationCalculations_tests, GetAnimationOffsets_twoBonesHaveAnimDataThirdDoesNot_thirdHasNoValue)
{
    auto decomposedMatrices = nc::graphics::GetAnimationOffsets(0.0f, testData->boneNames, &testData->animation);
    EXPECT_EQ(decomposedMatrices.hasValues[0], 1);
    EXPECT_EQ(decomposedMatrices.hasValues[1], 1);
    EXPECT_EQ(decomposedMatrices.hasValues[2], 0);
}

TEST_F(SkeletalAnimationCalculations_tests, ComposeMatrices_twoBonesHaveAnimData_bothHaveValues)
{
    auto matrices = nc::graphics::ComposeMatrices(0.0f, testData->boneNames, &testData->animation);
    EXPECT_EQ(matrices.hasValues[0], 1);
    EXPECT_EQ(matrices.hasValues[1], 1);
}

TEST_F(SkeletalAnimationCalculations_tests, ComposeMatrices_twoBonesHaveAnimDataThirdDoesNot_thirdHasNoValue)
{
    auto matrices = nc::graphics::ComposeMatrices(0.0f, testData->boneNames, &testData->animation);
    EXPECT_EQ(matrices.hasValues[0], 1);
    EXPECT_EQ(matrices.hasValues[1], 1);
    EXPECT_EQ(matrices.hasValues[2], 0);
}

TEST_F(SkeletalAnimationCalculations_tests, ComposeBlendedMatrices_twoBonesHaveAnimData_bothHaveValues)
{
    auto matrices = nc::graphics::ComposeBlendedMatrices(0.0f, 0.0f, 1.0f, testData->boneNames, &testData->animation, &testData->animation);
    EXPECT_EQ(matrices.hasValues[0], 1);
    EXPECT_EQ(matrices.hasValues[1], 1);
}

TEST_F(SkeletalAnimationCalculations_tests, ComposeBlendedMatrices_twoBonesHaveAnimDataThirdDoesNot_thirdHasNoValue)
{
    auto matrices = nc::graphics::ComposeBlendedMatrices(0.0f, 0.0f, 1.0f, testData->boneNames, &testData->animation, &testData->animation);
    EXPECT_EQ(matrices.hasValues[0], 1);
    EXPECT_EQ(matrices.hasValues[1], 1);
    EXPECT_EQ(matrices.hasValues[2], 0);
}

TEST_F(SkeletalAnimationCalculations_tests, ComposeBlendedMatrices_twoBonesHaveAnimDataThirdDoesNot_thirdHasNoValue)
{
    auto matrices = nc::graphics::ComposeMatrices(0.0f, testData->boneNames, &testData->animation);
    EXPECT_EQ(matrices.hasValues[0], 1);
    EXPECT_EQ(matrices.hasValues[1], 1);
    EXPECT_EQ(matrices.hasValues[2], 0);
}