#include "gtest/gtest.h"
#include "component/Transform.h"

#include <iostream>

using namespace nc;

auto TestHandle = EntityHandle{1u};
auto TestHandle2 = EntityHandle{2u};
auto TestHandle3 = EntityHandle{3u};
auto TestPos1 = Vector3{0.0f, 1.0f, 2.0f};
auto TestPos2 = Vector3{1.5f, 1.5f, 1.5f};
auto TestRotQuat1 = Quaternion::Identity();
auto TestRotQuat2 = Quaternion::FromEulerAngles(1.5f, 1.5f, 1.5f);
auto TestRotQuat3 = Quaternion::FromEulerAngles(2.5, 0.0f, 1.0f);
auto TestRotAngles = Vector3{0.15f, 1.0f, 0.177f};
auto TestScale1 = Vector3::One();
auto TestScale2 = Vector3{10.0f, 1.0f, 3.7f};
auto TestScale3 = Vector3::Splat(2.0f);

TEST(Transform_unit_tests, GetLocalPosition_CalledFromRoot_DecomposesPositionFromMatrix)
{
    auto t = Transform{TestHandle, TestPos1, TestRotQuat1, TestScale1, nullptr};
    auto actual = t.GetLocalPosition();
    EXPECT_EQ(actual, TestPos1);
}

TEST(Transform_unit_test, GetLocalPosition_CalledFromChild_ResultIsRelativeToParent)
{
    auto parent = Transform{TestHandle, TestPos1, TestRotQuat1, TestScale1, nullptr};
    auto child = Transform{TestHandle2, Vector3::Zero(), Quaternion::Identity(), Vector3::One(), &parent};
    auto actual = child.GetLocalPosition();
    EXPECT_EQ(actual, Vector3::Zero());
}

TEST(Transform_unit_tests, GetPosition_CalledFromRoot_DecomposesPositionFromMatrix)
{
    auto t = Transform{TestHandle, TestPos1, TestRotQuat1, TestScale1, nullptr};
    auto actual = t.GetPosition();
    EXPECT_EQ(actual, TestPos1);
}

TEST(Transform_unit_tests, GetPosition_CalledFromChild_ResultIncludesParentPosition)
{
    auto parent = Transform{TestHandle, TestPos1, TestRotQuat1, TestScale1, nullptr};
    auto child = Transform{TestHandle2, Vector3::Zero(), Quaternion::Identity(), Vector3::One(), &parent};
    auto actual = child.GetPosition();
    EXPECT_EQ(actual, TestPos1);
}

TEST(Transform_unit_tests, GetLocalRotation_CalledFromRoot_DecomposesRotationFromMatrix)
{
    auto t = Transform{TestHandle, TestPos1, TestRotQuat1, TestScale1, nullptr};
    auto actual = t.GetLocalRotation();
    EXPECT_EQ(actual, TestRotQuat1);
}

TEST(Transform_unit_tests, GetLocalRotation_CalledFromChild_ResultIsRelativeToParent)
{
    auto parent = Transform{TestHandle, TestPos1, TestRotQuat2, TestScale1, nullptr};
    auto child = Transform{TestHandle2, TestPos1, TestRotQuat2, TestScale2, &parent};
    auto actual = child.GetLocalRotation();
    EXPECT_EQ(actual, TestRotQuat2);
}

TEST(Transform_unit_tests, GetRotation_CalledFromRoot_DecomposesRotationFromMatrix)
{
    auto t = Transform{TestHandle, TestPos1, TestRotQuat2, TestScale1, nullptr};
    auto actual = t.GetRotation();
    EXPECT_EQ(actual, TestRotQuat2);
}

TEST(Transform_unit_tests, GetRotation_CalledFromChild_ResultIncludesParentRotation)
{
    auto parent = Transform{TestHandle, TestPos1, TestRotQuat2, TestScale2, nullptr};
    auto child = Transform{TestHandle2, TestPos2, TestRotQuat3, TestScale3, &parent};
    auto expected = Multiply(TestRotQuat2, TestRotQuat3);
    auto actual = child.GetRotation();
    EXPECT_EQ(actual, expected);
}

TEST(Transform_unit_tests, GetLocalScale_CalledFromRoot_DecomposesScaleFromMatrix)
{
    auto t = Transform{TestHandle, TestPos1, TestRotQuat1, TestScale1, nullptr};
    auto actual = t.GetLocalScale();
    EXPECT_EQ(actual, TestScale1);
}

TEST(Transform_unit_tests, GetLocalScale_CalledFromChild_ResultIsRelativeToParent)
{
    auto parent = Transform{TestHandle, TestPos1, TestRotQuat1, TestScale2, nullptr};
    auto child = Transform{TestHandle, TestPos1, TestRotQuat1, TestScale3, &parent};
    auto actual = child.GetLocalScale();
    EXPECT_EQ(actual, TestScale3);
}

TEST(Transform_unit_tests, GetScale_CalledFromRoot_DecomposesScaleFromMatrix)
{
    auto t = Transform{TestHandle, TestPos1, TestRotQuat1, TestScale1, nullptr};
    auto actual = t.GetScale();
    EXPECT_EQ(actual, TestScale1);
}

TEST(Transform_unit_tests, GetScale_CalledFromChild_ResultIncludesParentScale)
{
    auto parent = Transform{TestHandle, TestPos1, TestRotQuat1, TestScale2, nullptr};
    auto child = Transform{TestHandle, TestPos1, TestRotQuat1, TestScale3, &parent};
    auto actual = child.GetScale();
    auto expected = HadamardProduct(TestScale2, TestScale3);
    EXPECT_EQ(actual, expected);
}

TEST(Transform_unit_tests, Up_ReturnsNormalizedVector)
{
    auto t = Transform{TestHandle, TestPos1, TestRotQuat2, TestScale2, nullptr};
    auto actual = t.Up();
    auto expected = Normalize(actual);
    EXPECT_EQ(actual, expected);
}

TEST(Transform_unit_tests, Up_RotatedTransform_ReturnsCorrectAxis)
{
    auto rot = Quaternion::FromEulerAngles(math::DegreesToRadians(90.0f), 0.0f, 0.0f);
    auto t = Transform{TestHandle, Vector3::Zero(), rot, Vector3::One(), nullptr};
    auto actual = t.Up();
    auto expected = Vector3::Front();
    EXPECT_EQ(actual, expected);
}

TEST(Transform_unit_tests, Up_RotatedParent_ChildReturnsCorrectAxis)
{
    auto rot = Quaternion::FromEulerAngles(math::DegreesToRadians(90.0f), 0.0f, 0.0f);
    auto parent = Transform{TestHandle, Vector3::Zero(), rot, Vector3::One(), nullptr};
    auto child = Transform{TestHandle2, Vector3::Zero(), Quaternion::Identity(), Vector3::One(), &parent};
    auto actual = child.Up();
    auto expected = Vector3::Front();
    EXPECT_EQ(actual, expected);
}

TEST(Transform_unit_tests, Forward_ReturnsNormalizedVector)
{
    auto t = Transform{TestHandle, TestPos1, TestRotQuat2, TestScale2, nullptr};
    auto actual = t.Forward();
    auto expected = Normalize(actual);
    EXPECT_EQ(actual, expected);
}

TEST(Transform_unit_tests, Forward_RotatedTransform_ReturnsCorrectAxis)
{
    auto rot = Quaternion::FromEulerAngles(math::DegreesToRadians(90.0f), 0.0f, 0.0f);
    auto t = Transform{TestHandle, Vector3::Zero(), rot, Vector3::One(), nullptr};
    auto actual = t.Forward();
    auto expected = Vector3::Down();
    EXPECT_EQ(actual, expected);
}

TEST(Transform_unit_tests, Forward_RotatedParent_ChildReturnsCorrectAxis)
{
    auto rot = Quaternion::FromEulerAngles(math::DegreesToRadians(90.0f), 0.0f, 0.0f);
    auto parent = Transform{TestHandle, Vector3::Zero(), rot, Vector3::One(), nullptr};
    auto child = Transform{TestHandle2, Vector3::Zero(), Quaternion::Identity(), Vector3::One(), &parent};
    auto actual = child.Forward();
    auto expected = Vector3::Down();
    EXPECT_EQ(actual, expected);
}

TEST(Transform_unit_tests, Right_ReturnsNormalizedVector)
{
    auto t = Transform{TestHandle, TestPos1, TestRotQuat2, TestScale2, nullptr};
    auto actual = t.Right();
    auto expected = Normalize(actual);
    EXPECT_EQ(actual, expected);
}

TEST(Transform_unit_tests, Right_RotatedTransform_ReturnsCorrectAxis)
{
    auto rot = Quaternion::FromEulerAngles(math::DegreesToRadians(90.0f), 0.0f, 0.0f);
    auto t = Transform{TestHandle, Vector3::Zero(), rot, Vector3::One(), nullptr};
    auto actual = t.Right();
    auto expected = Vector3::Right();
    EXPECT_EQ(actual, expected);
}

TEST(Transform_unit_tests, Right_RotatedParent_ChildReturnsCorrectAxis)
{
    auto rot = Quaternion::FromEulerAngles(math::DegreesToRadians(90.0f), 0.0f, 0.0f);
    auto parent = Transform{TestHandle, Vector3::Zero(), rot, Vector3::One(), nullptr};
    auto child = Transform{TestHandle2, Vector3::Zero(), Quaternion::Identity(), Vector3::One(), &parent};
    auto actual = child.Right();
    auto expected = Vector3::Right();
    EXPECT_EQ(actual, expected);
}

TEST(Transform_unit_tests, Set_CalledOnRoot_CorrectlyRecomposesBothMatrices)
{
    auto t = Transform{TestHandle, TestPos1, TestRotQuat1, TestScale1, nullptr};
    t.Set(TestPos2, TestRotQuat2, TestScale2);
    EXPECT_EQ(t.GetPosition(), TestPos2);
    EXPECT_EQ(t.GetRotation(), TestRotQuat2);
    EXPECT_EQ(t.GetScale(), TestScale2);
    EXPECT_EQ(t.GetLocalPosition(), TestPos2);
    EXPECT_EQ(t.GetLocalRotation(), TestRotQuat2);
    EXPECT_EQ(t.GetLocalScale(), TestScale2);
}

TEST(Transform_unit_tests, Set_CalledOnParent_OnlyRecomposesChildWorldMatrix)
{
    auto parent = Transform{TestHandle, TestPos1, TestRotQuat1, TestScale1, nullptr};
    auto child = Transform{TestHandle2, Vector3::Zero(), Quaternion::Identity(), Vector3::One(), &parent};
    parent.Set(TestPos2, TestRotQuat2, TestScale2);
    EXPECT_EQ(child.GetPosition(), TestPos2);
    EXPECT_EQ(child.GetRotation(), TestRotQuat2);
    EXPECT_EQ(child.GetScale(), TestScale2);
    EXPECT_EQ(child.GetLocalPosition(), Vector3::Zero());
    EXPECT_EQ(child.GetLocalRotation(), Quaternion::Identity());
    EXPECT_EQ(child.GetLocalScale(), Vector3::One());
}

TEST(Transform_unit_tests, SetAngleOverload_CalledOnRoot_CorrectlyRecomposesBothMatrices)
{
    auto t = Transform{TestHandle, TestPos1, TestRotQuat1, TestScale1, nullptr};
    t.Set(TestPos2, TestRotAngles, TestScale2);
    EXPECT_EQ(t.GetPosition(), TestPos2);
    auto expectedRot = Quaternion::FromEulerAngles(TestRotAngles);
    EXPECT_EQ(t.GetRotation(), expectedRot);
    EXPECT_EQ(t.GetScale(), TestScale2);
    EXPECT_EQ(t.GetLocalPosition(), TestPos2);
    EXPECT_EQ(t.GetLocalRotation(), expectedRot);
    EXPECT_EQ(t.GetLocalScale(), TestScale2);
}

TEST(Transform_unit_tests, SetAngleOverload_CalledOnParent_OnlyRecomposesChildWorldMatrix)
{
    auto parent = Transform{TestHandle, TestPos1, TestRotQuat1, TestScale1, nullptr};
    auto child = Transform{TestHandle2, Vector3::Zero(), Quaternion::Identity(), Vector3::One(), &parent};
    parent.Set(TestPos2, TestRotAngles, TestScale2);
    EXPECT_EQ(child.GetPosition(), TestPos2);
    auto expectedRot = Quaternion::FromEulerAngles(TestRotAngles);
    EXPECT_EQ(child.GetRotation(), expectedRot);
    EXPECT_EQ(child.GetScale(), TestScale2);
    EXPECT_EQ(child.GetLocalPosition(), Vector3::Zero());
    EXPECT_EQ(child.GetLocalRotation(), Quaternion::Identity());
    EXPECT_EQ(child.GetLocalScale(), Vector3::One());
}

TEST(Transform_unit_tests, SetPosition_CalledOnRoot_OnlyPositionModified)
{
    auto t = Transform{TestHandle, TestPos1, TestRotQuat1, TestScale1, nullptr};
    t.SetPosition(TestPos2);
    EXPECT_EQ(t.GetPosition(), TestPos2);
    EXPECT_EQ(t.GetRotation(), TestRotQuat1);
    EXPECT_EQ(t.GetScale(), TestScale1);
    EXPECT_EQ(t.GetLocalPosition(), TestPos2);
    EXPECT_EQ(t.GetLocalRotation(), TestRotQuat1);
    EXPECT_EQ(t.GetLocalScale(), TestScale1);
}

TEST(Transform_unit_tests, SetPosition_CalledOnParent_OnlyWorldPositionModifiedInChild)
{
    auto parent = Transform{TestHandle, TestPos1, TestRotQuat1, TestScale1, nullptr};
    auto child = Transform{TestHandle2, Vector3::Zero(), Quaternion::Identity(), Vector3::One(), &parent};
    parent.SetPosition(TestPos2);
    EXPECT_EQ(child.GetPosition(), TestPos2);
    EXPECT_EQ(child.GetRotation(), TestRotQuat1);
    EXPECT_EQ(child.GetScale(), TestScale1);
    EXPECT_EQ(child.GetLocalPosition(), Vector3::Zero());
    EXPECT_EQ(child.GetLocalRotation(), Quaternion::Identity());
    EXPECT_EQ(child.GetLocalScale(), Vector3::One());
}

TEST(Transform_unit_tests, SetRotation_CalledOnRoot_OnlyRotationModified)
{
    auto t = Transform{TestHandle, TestPos1, TestRotQuat1, TestScale1, nullptr};
    t.SetRotation(TestRotQuat2);
    EXPECT_EQ(t.GetPosition(), TestPos1);
    EXPECT_EQ(t.GetRotation(), TestRotQuat2);
    EXPECT_EQ(t.GetScale(), TestScale1);
    EXPECT_EQ(t.GetLocalPosition(), TestPos1);
    EXPECT_EQ(t.GetLocalRotation(), TestRotQuat2);
    EXPECT_EQ(t.GetLocalScale(), TestScale1);
}

TEST(Transform_unit_tests, SetRotation_CalledOnParent_OnlyWorldRotationModifiedInChild)
{
    auto parent = Transform{TestHandle, TestPos1, TestRotQuat1, TestScale1, nullptr};
    auto child = Transform{TestHandle2, Vector3::Zero(), Quaternion::Identity(), Vector3::One(), &parent};
    parent.SetRotation(TestRotQuat2);
    EXPECT_EQ(child.GetPosition(), TestPos1);
    EXPECT_EQ(child.GetRotation(), TestRotQuat2);
    EXPECT_EQ(child.GetScale(), TestScale1);
    EXPECT_EQ(child.GetLocalPosition(), Vector3::Zero());
    EXPECT_EQ(child.GetLocalRotation(), Quaternion::Identity());
    EXPECT_EQ(child.GetLocalScale(), Vector3::One());
}

TEST(Transform_unit_tests, SetRotationAnglesOverload_CalledOnRoot_OnlyRotationModified)
{
    auto t = Transform{TestHandle, TestPos1, TestRotQuat1, TestScale1, nullptr};
    t.SetRotation(TestRotAngles);
    EXPECT_EQ(t.GetPosition(), TestPos1);
    auto expectedRot = Quaternion::FromEulerAngles(TestRotAngles);
    EXPECT_EQ(t.GetRotation(), expectedRot);
    EXPECT_EQ(t.GetScale(), TestScale1);
    EXPECT_EQ(t.GetLocalPosition(), TestPos1);
    EXPECT_EQ(t.GetLocalRotation(), expectedRot);
    EXPECT_EQ(t.GetLocalScale(), TestScale1);
}

TEST(Transform_unit_tests, SetRotationAnglesOverload_CalledOnParent_OnlyWorldRotationModifiedInChild)
{
    auto parent = Transform{TestHandle, TestPos1, TestRotQuat1, TestScale1, nullptr};
    auto child = Transform{TestHandle2, Vector3::Zero(), Quaternion::Identity(), Vector3::One(), &parent};
    parent.SetRotation(TestRotAngles);
    EXPECT_EQ(child.GetPosition(), TestPos1);
    auto expectedRot = Quaternion::FromEulerAngles(TestRotAngles);
    EXPECT_EQ(child.GetRotation(), expectedRot);
    EXPECT_EQ(child.GetScale(), TestScale1);
    EXPECT_EQ(child.GetLocalPosition(), Vector3::Zero());
    EXPECT_EQ(child.GetLocalRotation(), Quaternion::Identity());
    EXPECT_EQ(child.GetLocalScale(), Vector3::One());
}

TEST(Transform_unit_tests, SetScale_CalledOnRoot_OnlyScaleModified)
{
    auto t = Transform{TestHandle, TestPos1, TestRotQuat1, TestScale1, nullptr};
    t.SetScale(TestScale2);
    EXPECT_EQ(t.GetPosition(), TestPos1);
    EXPECT_EQ(t.GetRotation(), TestRotQuat1);
    EXPECT_EQ(t.GetScale(), TestScale2);
    EXPECT_EQ(t.GetLocalPosition(), TestPos1);
    EXPECT_EQ(t.GetLocalRotation(), TestRotQuat1);
    EXPECT_EQ(t.GetLocalScale(), TestScale2);
}

TEST(Transform_unit_tests, SetScale_CalledOnParent_OnlyWorldScaleModifiedInChild)
{
    auto parent = Transform{TestHandle, TestPos1, TestRotQuat1, TestScale1, nullptr};
    auto child = Transform{TestHandle2, Vector3::Zero(), Quaternion::Identity(), Vector3::One(), &parent};
    parent.SetScale(TestScale2);
    EXPECT_EQ(child.GetPosition(), TestPos1);
    EXPECT_EQ(child.GetRotation(), TestRotQuat1);
    EXPECT_EQ(child.GetScale(), TestScale2);
    EXPECT_EQ(child.GetLocalPosition(), Vector3::Zero());
    EXPECT_EQ(child.GetLocalRotation(), Quaternion::Identity());
    EXPECT_EQ(child.GetLocalScale(), Vector3::One());
}

TEST(Transform_unit_tests, Translate_CalledOnRoot_OnlyPositionModified)
{
    auto t = Transform{TestHandle, TestPos1, TestRotQuat1, TestScale1, nullptr};
    t.Translate(TestPos1);
    auto expectedPos = TestPos1 + TestPos1;
    EXPECT_EQ(t.GetPosition(), expectedPos);
    EXPECT_EQ(t.GetRotation(), TestRotQuat1);
    EXPECT_EQ(t.GetScale(), TestScale1);
    EXPECT_EQ(t.GetLocalPosition(), expectedPos);
    EXPECT_EQ(t.GetLocalRotation(), TestRotQuat1);
    EXPECT_EQ(t.GetLocalScale(), TestScale1);
}

TEST(Transform_unit_tests, Translate_CalledOnParent_OnlyWorldPositionModifiedInChild)
{
    auto parent = Transform{TestHandle, TestPos1, TestRotQuat1, TestScale1, nullptr};
    auto child = Transform{TestHandle2, Vector3::Zero(), Quaternion::Identity(), Vector3::One(), &parent};
    parent.Translate(TestPos1);
    auto expectedPos = TestPos1 + TestPos1;
    EXPECT_EQ(child.GetPosition(), expectedPos);
    EXPECT_EQ(child.GetRotation(), TestRotQuat1);
    EXPECT_EQ(child.GetScale(), TestScale1);
    EXPECT_EQ(child.GetLocalPosition(), Vector3::Zero());
    EXPECT_EQ(child.GetLocalRotation(), Quaternion::Identity());
    EXPECT_EQ(child.GetLocalScale(), Vector3::One());
}

TEST(Transform_unit_tests, TranslateLocalSpace_CalledOnRoot_OnlyPositionModified)
{
    auto t = Transform{TestHandle, TestPos1, Quaternion::Identity(), TestScale1, nullptr};
    t.TranslateLocalSpace(TestPos1);
    auto expectedPos = TestPos1 + TestPos1;
    EXPECT_EQ(t.GetPosition(), expectedPos);
    EXPECT_EQ(t.GetRotation(), Quaternion::Identity());
    EXPECT_EQ(t.GetScale(), TestScale1);
    EXPECT_EQ(t.GetLocalPosition(), expectedPos);
    EXPECT_EQ(t.GetLocalRotation(), Quaternion::Identity());
    EXPECT_EQ(t.GetLocalScale(), TestScale1);
}

TEST(Transform_unit_tests, TranslateLocalSpace_CalledOnParent_OnlyWorldPositionModifiedInChild)
{
    auto parent = Transform{TestHandle, TestPos1, Quaternion::Identity(), TestScale1, nullptr};
    auto child = Transform{TestHandle2, Vector3::Zero(), Quaternion::Identity(), Vector3::One(), &parent};
    parent.TranslateLocalSpace(TestPos1);
    auto expectedPos = TestPos1 + TestPos1;
    EXPECT_EQ(child.GetPosition(), expectedPos);
    EXPECT_EQ(child.GetRotation(), Quaternion::Identity());
    EXPECT_EQ(child.GetScale(), TestScale1);
    EXPECT_EQ(child.GetLocalPosition(), Vector3::Zero());
    EXPECT_EQ(child.GetLocalRotation(), Quaternion::Identity());
    EXPECT_EQ(child.GetLocalScale(), Vector3::One());
}

TEST(Transform_unit_tests, Rotate_CalledOnRoot_OnlyRotationModified)
{
    auto t = Transform{TestHandle, TestPos1, Quaternion::Identity(), TestScale1, nullptr};
    t.Rotate(TestRotQuat2);
    EXPECT_EQ(t.GetPosition(), TestPos1);
    EXPECT_EQ(t.GetRotation(), TestRotQuat2);
    EXPECT_EQ(t.GetScale(), TestScale1);
    EXPECT_EQ(t.GetLocalPosition(), TestPos1);
    EXPECT_EQ(t.GetLocalRotation(), TestRotQuat2);
    EXPECT_EQ(t.GetLocalScale(), TestScale1);
}

TEST(Transform_unit_tests, Rotate_CalledOnParent_OnlyWorldRotationModifiedInChild)
{
    auto parent = Transform{TestHandle, TestPos1, Quaternion::Identity(), TestScale1, nullptr};
    auto child = Transform{TestHandle2, Vector3::Zero(), Quaternion::Identity(), Vector3::One(), &parent};
    parent.Rotate(TestRotQuat2);
    EXPECT_EQ(child.GetPosition(), TestPos1);
    EXPECT_EQ(child.GetRotation(), TestRotQuat2);
    EXPECT_EQ(child.GetScale(), TestScale1);
    EXPECT_EQ(child.GetLocalPosition(), Vector3::Zero());
    EXPECT_EQ(child.GetLocalRotation(), Quaternion::Identity());
    EXPECT_EQ(child.GetLocalScale(), Vector3::One());
}

TEST(Transform_unit_tests, RotateAxisAngleOverload_CalledOnRoot_OnlyRotationModified)
{
    auto t = Transform{TestHandle, TestPos1, Quaternion::Identity(), TestScale1, nullptr};
    auto expectedAxis = Vector3::Up();
    auto expectedAngle = 2.2f; 
    t.Rotate(expectedAxis, expectedAngle);
    auto actualQuat = t.GetRotation();
    auto expectedQuat = Quaternion::FromAxisAngle(expectedAxis, expectedAngle);
    EXPECT_EQ(t.GetPosition(), TestPos1);
    EXPECT_EQ(actualQuat, expectedQuat);
    EXPECT_EQ(t.GetScale(), TestScale1);
    EXPECT_EQ(t.GetLocalPosition(), TestPos1);
    EXPECT_EQ(t.GetLocalRotation(), expectedQuat);
    EXPECT_EQ(t.GetLocalScale(), TestScale1);
}

TEST(Transform_unit_tests, RotateAxisAngleOverload_CalledOnParent_OnlyWorldRotationModifiedInChild)
{
    auto parent = Transform{TestHandle, TestPos1, Quaternion::Identity(), TestScale1, nullptr};
    auto child = Transform{TestHandle2, Vector3::Zero(), Quaternion::Identity(), Vector3::One(), &parent};
    auto expectedAxis = Vector3::Up();
    auto expectedAngle = 2.2f; 
    parent.Rotate(expectedAxis, expectedAngle);
    EXPECT_EQ(child.GetPosition(), TestPos1);
    auto expectedQuat = Quaternion::FromAxisAngle(expectedAxis, expectedAngle);
    auto actualQuat = child.GetRotation();
    EXPECT_EQ(actualQuat, expectedQuat);
    EXPECT_EQ(child.GetScale(), TestScale1);
    EXPECT_EQ(child.GetLocalPosition(), Vector3::Zero());
    EXPECT_EQ(child.GetLocalRotation(), Quaternion::Identity());
    EXPECT_EQ(child.GetLocalScale(), Vector3::One());
}

TEST(Transform_unit_tests, GetChildren_ChildConstructor_AddsChildToParent)
{
    auto parent = Transform{TestHandle, TestPos1, Quaternion::Identity(), TestScale1, nullptr};
    auto child = Transform{TestHandle2, Vector3::Zero(), Quaternion::Identity(), Vector3::One(), &parent};
    auto children = parent.GetChildren();
    auto actualCount = children.size();
    ASSERT_EQ(actualCount, 1u);
    EXPECT_EQ(children[0], &child);
}

TEST(Transform_unit_tests, GetRoot_CalledOnRoot_ReturnsSelf)
{
    auto root = Transform{TestHandle, Vector3::Zero(), Quaternion::Identity(), Vector3::One(), nullptr};
    auto actual = root.GetRoot();
    EXPECT_EQ(actual, &root);
}

TEST(Transform_unit_tests, GetRoot_WithOneDepthLevel_ReturnsParent)
{
    auto parent = Transform{TestHandle, Vector3::Zero(), Quaternion::Identity(), Vector3::One(), nullptr};
    auto child = Transform{TestHandle2, Vector3::Zero(), Quaternion::Identity(), Vector3::One(), &parent};
    auto actual = child.GetRoot();
    EXPECT_EQ(actual, &parent);
}

TEST(Transform_unit_tests, GetRoot_WithTwoDepthLevels_ReturnsParentsParent)
{
    auto root = Transform{TestHandle, Vector3::Zero(), Quaternion::Identity(), Vector3::One(), nullptr};
    auto parent = Transform{TestHandle2, Vector3::Zero(), Quaternion::Identity(), Vector3::One(), &root};
    auto child = Transform{TestHandle3, Vector3::Zero(), Quaternion::Identity(), Vector3::One(), &parent};
    auto actual = child.GetRoot();
    EXPECT_EQ(actual, &root);
}

TEST(Transform_unit_tests, SetParent_RootChangedToChild_ParentAndChildUpdated)
{
    auto parent = Transform{TestHandle, Vector3::Zero(), Quaternion::Identity(), Vector3::One(), nullptr};
    auto child = Transform{TestHandle2, Vector3::Zero(), Quaternion::Identity(), Vector3::One(), nullptr};
    child.SetParent(&parent);
    EXPECT_EQ(child.GetParent(), &parent);
    auto children = parent.GetChildren();
    ASSERT_EQ(children.size(), 1u);
    EXPECT_EQ(children[0], &child);
}

TEST(Transform_unit_tests, SetParent_ChildReParented_ParentAndChildUpdated)
{
    auto initialParent = Transform{TestHandle, Vector3::Zero(), Quaternion::Identity(), Vector3::One(), nullptr};
    auto newParent = Transform{TestHandle2, Vector3::Zero(), Quaternion::Identity(), Vector3::One(), nullptr};
    auto child = Transform{TestHandle3, Vector3::Zero(), Quaternion::Identity(), Vector3::One(), &initialParent};
    child.SetParent(&newParent);
    EXPECT_EQ(child.GetParent(), &newParent);
    auto initialParentChildren = initialParent.GetChildren();
    EXPECT_EQ(initialParentChildren.size(), 0u);
    auto newParentChildren = newParent.GetChildren();
    ASSERT_EQ(newParentChildren.size(), 1u);
    EXPECT_EQ(newParentChildren[0], &child);
}

TEST(Transform_unit_tests, SetParent_ChildDetached_ParentAndChildUpdated)
{
    auto parent = Transform{TestHandle, Vector3::Zero(), Quaternion::Identity(), Vector3::One(), nullptr};
    auto child = Transform{TestHandle2, Vector3::Zero(), Quaternion::Identity(), Vector3::One(), &parent};
    child.SetParent(nullptr);
    EXPECT_EQ(child.GetParent(), nullptr);
    auto children = parent.GetChildren();
    EXPECT_EQ(children.size(), 0u);
}

int main(int argc, char ** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}