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
    return DirectX::XMVector4Equal(a.r[0], b.r[0]) &&
           DirectX::XMVector4Equal(a.r[1], b.r[1]) &&
           DirectX::XMVector4Equal(a.r[2], b.r[2]) &&
           DirectX::XMVector4Equal(a.r[3], b.r[3]);
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
        .rotation = nc::Quaternion::FromEulerAngles(0.0f, 0.0f, 0.0f)
    };

    auto rotationFrame2 = nc::asset::RotationFrame
    {
        .timeInTicks = 2.0f,
        .rotation = nc::Quaternion::FromEulerAngles(3.14f, 0.0f, 0.0f)
    };

    auto rotationFrames = std::vector<nc::asset::RotationFrame>
    {
        rotationFrame,
        rotationFrame2
    };

    auto result = graphics::GetInterpolatedRotation(1.0f, rotationFrames);
    auto expected = nc::Quaternion::FromEulerAngles(1.57f, 0.0f, 0.0f);
    EXPECT_FLOAT_EQ(result.x, expected.x);
    EXPECT_FLOAT_EQ(result.y, expected.y);
    EXPECT_FLOAT_EQ(result.z, expected.z);
    EXPECT_FLOAT_EQ(result.w, expected.w);
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
