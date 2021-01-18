#include "gtest/gtest.h"
#include "component/Transform.h"

#include <iostream>

using namespace nc;

auto TestHandle = EntityHandle{1u};
auto TestPos1 = Vector3{0.0f, 1.0f, 2.0f};
auto TestPos2 = Vector3{1.5f, 1.5f, 1.5f};
auto TestRotQuat1 = Quaternion::Identity();
auto TestRotQuat2 = Quaternion::FromEulerAngles(1.5f, 1.5f, 1.5f);
auto TestRotAngles = Vector3{0.15f, 1.0f, 0.177f};
auto TestScale1 = Vector3::One();
auto TestScale2 = Vector3{10.0f, 1.0f, 3.7f};

TEST(Transform_unit_tests, GetPosition_DecomposesPositionFromMatrix)
{
    auto t = Transform{TestHandle, TestPos1, TestRotQuat1, TestScale1};
    auto actual = t.GetPosition();
    EXPECT_EQ(actual, TestPos1);
}

TEST(Transform_unit_tests, GetRotation_DecomposesRotationFromMatrix)
{
    auto t = Transform{TestHandle, TestPos1, TestRotQuat1, TestScale1};
    auto actual = t.GetRotation();
    EXPECT_EQ(actual, TestRotQuat1);
}

TEST(Transform_unit_tests, GetScale_DecomposesScaleFromMatrix)
{
    auto t = Transform{TestHandle, TestPos1, TestRotQuat1, TestScale1};
    auto actual = t.GetScale();
    EXPECT_EQ(actual, TestScale1);
}

TEST(Transform_unit_tests, Set_CorrectlyRecomposesMatrix)
{
    auto t = Transform{TestHandle, TestPos1, TestRotQuat1, TestScale1};
    t.Set(TestPos2, TestRotQuat2, TestScale2);
    EXPECT_EQ(t.GetPosition(), TestPos2);
    EXPECT_EQ(t.GetRotation(), TestRotQuat2);
    EXPECT_EQ(t.GetScale(), TestScale2);
}

TEST(Transform_unit_tests, Set_AngleOverload_CorrectlyRecomposesMatrix)
{
    auto t = Transform{TestHandle, TestPos1, TestRotQuat1, TestScale1};
    t.Set(TestPos2, TestRotAngles, TestScale2);
    EXPECT_EQ(t.GetPosition(), TestPos2);
    auto expectedRot = Quaternion::FromEulerAngles(TestRotAngles);
    EXPECT_EQ(t.GetRotation(), expectedRot);
    EXPECT_EQ(t.GetScale(), TestScale2);
}

TEST(Transform_unit_tests, SetPosition_OnlyModifiesPositionMatrixElements)
{
    auto t = Transform{TestHandle, TestPos1, TestRotQuat1, TestScale1};
    t.SetPosition(TestPos2);
    EXPECT_EQ(t.GetPosition(), TestPos2);
    EXPECT_EQ(t.GetRotation(), TestRotQuat1);
    EXPECT_EQ(t.GetScale(), TestScale1);
}

TEST(Transform_unit_tests, SetRotation_OnlyModifiesRotationMatrixElements)
{
    auto t = Transform{TestHandle, TestPos1, TestRotQuat1, TestScale1};
    t.SetRotation(TestRotQuat2);
    EXPECT_EQ(t.GetPosition(), TestPos1);
    EXPECT_EQ(t.GetRotation(), TestRotQuat2);
    EXPECT_EQ(t.GetScale(), TestScale1);
}

TEST(Transform_unit_tests, SetRotation_AnglesOverload_OnlyModifiesRotationMatrixElements)
{
    auto t = Transform{TestHandle, TestPos1, TestRotQuat1, TestScale1};
    t.SetRotation(TestRotAngles);
    EXPECT_EQ(t.GetPosition(), TestPos1);
    auto expectedRot = Quaternion::FromEulerAngles(TestRotAngles);
    EXPECT_EQ(t.GetRotation(), expectedRot);
    EXPECT_EQ(t.GetScale(), TestScale1);
}

TEST(Transform_unit_tests, SetScale_OnlyModifiesScaleMatrixElements)
{
    auto t = Transform{TestHandle, TestPos1, TestRotQuat1, TestScale1};
    t.SetScale(TestScale2);
    EXPECT_EQ(t.GetPosition(), TestPos1);
    EXPECT_EQ(t.GetRotation(), TestRotQuat1);
    EXPECT_EQ(t.GetScale(), TestScale2);
}

TEST(Transform_unit_tests, Translate_WorldSpace_OnlyModifiesTranslationMatrixElements)
{
    auto t = Transform{TestHandle, TestPos1, TestRotQuat1, TestScale1};
    t.Translate(TestPos1, Space::World);
    auto expectedPos = TestPos1 + TestPos1;
    EXPECT_EQ(t.GetPosition(), expectedPos);
    EXPECT_EQ(t.GetRotation(), TestRotQuat1);
    EXPECT_EQ(t.GetScale(), TestScale1);
}

TEST(Transform_unit_tests, Translate_LocalSpace_OnlyModifiesTranslationMatrixElements)
{
    auto t = Transform{TestHandle, TestPos1, Quaternion::Identity(), TestScale1};
    t.Translate(TestPos1, Space::Local);
    auto expectedPos = TestPos1 + TestPos1;
    EXPECT_EQ(t.GetPosition(), expectedPos);
    EXPECT_EQ(t.GetRotation(), Quaternion::Identity());
    EXPECT_EQ(t.GetScale(), TestScale1);
}

TEST(Transform_unit_tests, Rotate_OnlyModifiesTranslationMatrixElements)
{
    auto t = Transform{TestHandle, TestPos1, Quaternion::Identity(), TestScale1};
    t.Rotate(TestRotQuat2);
    EXPECT_EQ(t.GetPosition(), TestPos1);
    EXPECT_EQ(t.GetRotation(), TestRotQuat2);
    EXPECT_EQ(t.GetScale(), TestScale1);
}

TEST(Transform_unit_tests, Rotate_AxisAngleOverload_OnlyModifiesTranslationMatrixElements)
{
    auto t = Transform{TestHandle, TestPos1, Quaternion::Identity(), TestScale1};
    auto expectedAxis = Vector3::Up();
    auto expectedAngle = 2.2f; 
    t.Rotate(expectedAxis, expectedAngle);
    auto actualQuat = t.GetRotation();
    auto expectedQuat = Quaternion::FromAxisAngle(expectedAxis, expectedAngle);
    EXPECT_EQ(t.GetPosition(), TestPos1);
    EXPECT_EQ(actualQuat, expectedQuat);
    EXPECT_EQ(t.GetScale(), TestScale1);
}

int main(int argc, char ** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}