#include "gtest/gtest.h"
//#include "component/Transform.h"
//#include "Ecs.h"
//#include "ecs/Registry.h"

#include <iostream>

// using namespace nc;

// constexpr auto TestLayer = HandleTraits::layer_type{0u};
// constexpr auto TestVersion = HandleTraits::version_type{0u};
// constexpr auto TestFlags = EntityFlags::None;

// auto TestHandle = EntityHandle{1u, TestVersion, TestLayer, TestFlags};
// auto TestHandle2 = EntityHandle{2u, TestVersion, TestLayer, TestFlags};
// auto TestHandle3 = EntityHandle{3u, TestVersion, TestLayer, TestFlags};
// auto TestPos1 = Vector3{0.0f, 1.0f, 2.0f};
// auto TestPos2 = Vector3{1.5f, 1.5f, 1.5f};
// auto TestRotQuat1 = Quaternion::Identity();
// auto TestRotQuat2 = Quaternion::FromEulerAngles(1.5f, 1.5f, 1.5f);
// auto TestRotQuat3 = Quaternion::FromEulerAngles(2.5, 0.0f, 1.0f);
// auto TestRotAngles = Vector3{0.15f, 1.0f, 0.177f};
// auto TestScale1 = Vector3::One();
// auto TestScale2 = Vector3{10.0f, 1.0f, 3.7f};
// auto TestScale3 = Vector3::Splat(2.0f);

// ecs::Registry<Transform>* g_registry = nullptr;

// namespace nc
// {
//     template<>
//     Transform* GetComponent<Transform>(EntityHandle handle)
//     {
//         return g_registry->Get(handle);
//     }
// }

// class Transform_unit_tests : public ::testing::Test
// {
//     public:
//         ecs::Registry<Transform> registry;

//     protected:
//         Transform_unit_tests()
//             : registry{5u}
//         {
//             g_registry = &registry;
//         }

//         ~Transform_unit_tests()
//         {
//             registry.Clear();
//             g_registry = nullptr;
//         }
// };

// TEST_F(Transform_unit_tests, GetLocalPosition_CalledFromRoot_DecomposesPositionFromMatrix)
// {
//     auto* t = registry.Emplace(TestHandle, TestPos1, TestRotQuat1, TestScale1, EntityHandle::Null());
//     auto actual = t->GetLocalPosition();
//     EXPECT_EQ(actual, TestPos1);
// }

// TEST_F(Transform_unit_tests, GetLocalPosition_CalledFromChild_ResultIsRelativeToParent)
// {
//     registry.Emplace(TestHandle, TestPos1, TestRotQuat1, TestScale1, EntityHandle::Null());
//     auto* child = registry.Emplace(TestHandle2, Vector3::Zero(), Quaternion::Identity(), Vector3::One(), TestHandle);
//     auto actual = child->GetLocalPosition();
//     EXPECT_EQ(actual, Vector3::Zero());
// }

// TEST_F(Transform_unit_tests, GetPosition_CalledFromRoot_DecomposesPositionFromMatrix)
// {
//     auto* t = registry.Emplace(TestHandle, TestPos1, TestRotQuat1, TestScale1, EntityHandle::Null());
//     auto actual = t->GetPosition();
//     EXPECT_EQ(actual, TestPos1);
// }

// TEST_F(Transform_unit_tests, GetPosition_CalledFromChild_ResultIncludesParentPosition)
// {
//     registry.Emplace(TestHandle, TestPos1, TestRotQuat1, TestScale1, EntityHandle::Null());
//     auto* child = registry.Emplace(TestHandle2, Vector3::Zero(), Quaternion::Identity(), Vector3::One(), TestHandle);
//     auto actual = child->GetPosition();
//     EXPECT_EQ(actual, TestPos1);
// }

// TEST_F(Transform_unit_tests, GetLocalRotation_CalledFromRoot_DecomposesRotationFromMatrix)
// {
//     auto* t = registry.Emplace(TestHandle, TestPos1, TestRotQuat1, TestScale1, EntityHandle::Null());
//     auto actual = t->GetLocalRotation();
//     EXPECT_EQ(actual, TestRotQuat1);
// }

// TEST_F(Transform_unit_tests, GetLocalRotation_CalledFromChild_ResultIsRelativeToParent)
// {
//     registry.Emplace(TestHandle, TestPos1, TestRotQuat2, TestScale1, EntityHandle::Null());
//     auto* child = registry.Emplace(TestHandle2, TestPos1, TestRotQuat2, TestScale2, TestHandle);
//     auto actual = child->GetLocalRotation();
//     EXPECT_EQ(actual, TestRotQuat2);
// }

// TEST_F(Transform_unit_tests, GetRotation_CalledFromRoot_DecomposesRotationFromMatrix)
// {
//     auto* t = registry.Emplace(TestHandle, TestPos1, TestRotQuat2, TestScale1, EntityHandle::Null());
//     auto actual = t->GetRotation();
//     EXPECT_EQ(actual, TestRotQuat2);
// }

// TEST_F(Transform_unit_tests, GetRotation_CalledFromChild_ResultIncludesParentRotation)
// {
//     registry.Emplace(TestHandle, TestPos1, TestRotQuat2, TestScale2, EntityHandle::Null());
//     auto* child = registry.Emplace(TestHandle2, TestPos2, TestRotQuat3, TestScale3, TestHandle);
//     auto expected = Multiply(TestRotQuat2, TestRotQuat3);
//     auto actual = child->GetRotation();
//     EXPECT_EQ(actual, expected);
// }

// TEST_F(Transform_unit_tests, GetLocalScale_CalledFromRoot_DecomposesScaleFromMatrix)
// {
//     auto* t = registry.Emplace(TestHandle, TestPos1, TestRotQuat1, TestScale1, EntityHandle::Null());
//     auto actual = t->GetLocalScale();
//     EXPECT_EQ(actual, TestScale1);
// }

// TEST_F(Transform_unit_tests, GetLocalScale_CalledFromChild_ResultIsRelativeToParent)
// {
//     registry.Emplace(TestHandle, TestPos1, TestRotQuat1, TestScale2, EntityHandle::Null());
//     auto child = registry.Emplace(TestHandle2, TestPos1, TestRotQuat1, TestScale3, TestHandle);
//     auto actual = child->GetLocalScale();
//     EXPECT_EQ(actual, TestScale3);
// }

// TEST_F(Transform_unit_tests, GetScale_CalledFromRoot_DecomposesScaleFromMatrix)
// {
//     auto* t = registry.Emplace(TestHandle, TestPos1, TestRotQuat1, TestScale1, EntityHandle::Null());
//     auto actual = t->GetScale();
//     EXPECT_EQ(actual, TestScale1);
// }

// TEST_F(Transform_unit_tests, GetScale_CalledFromChild_ResultIncludesParentScale)
// {
//     registry.Emplace(TestHandle, TestPos1, TestRotQuat1, TestScale2, EntityHandle::Null());
//     auto* child = registry.Emplace(TestHandle2, TestPos1, TestRotQuat1, TestScale3, TestHandle);
//     auto actual = child->GetScale();
//     auto expected = HadamardProduct(TestScale2, TestScale3);
//     EXPECT_EQ(actual, expected);
// }

// TEST_F(Transform_unit_tests, Up_ReturnsNormalizedVector)
// {
//     auto* t = registry.Emplace(TestHandle, TestPos1, TestRotQuat2, TestScale2, EntityHandle::Null());
//     auto actual = t->Up();
//     auto expected = Normalize(actual);
//     EXPECT_EQ(actual, expected);
// }

// TEST_F(Transform_unit_tests, Up_RotatedTransform_ReturnsCorrectAxis)
// {
//     auto rot = Quaternion::FromEulerAngles(math::DegreesToRadians(90.0f), 0.0f, 0.0f);
//     auto* t = registry.Emplace(TestHandle, Vector3::Zero(), rot, Vector3::One(), EntityHandle::Null());
//     auto actual = t->Up();
//     auto expected = Vector3::Front();
//     EXPECT_EQ(actual, expected);
// }

// TEST_F(Transform_unit_tests, Up_RotatedParent_ChildReturnsCorrectAxis)
// {
//     auto rot = Quaternion::FromEulerAngles(math::DegreesToRadians(90.0f), 0.0f, 0.0f);
//     registry.Emplace(TestHandle, Vector3::Zero(), rot, Vector3::One(), EntityHandle::Null());
//     auto* child = registry.Emplace(TestHandle2, Vector3::Zero(), Quaternion::Identity(), Vector3::One(), TestHandle);
//     auto actual = child->Up();
//     auto expected = Vector3::Front();
//     EXPECT_EQ(actual, expected);
// }

// TEST_F(Transform_unit_tests, Forward_ReturnsNormalizedVector)
// {
//     auto* t = registry.Emplace(TestHandle, TestPos1, TestRotQuat2, TestScale2, EntityHandle::Null());
//     auto actual = t->Forward();
//     auto expected = Normalize(actual);
//     EXPECT_EQ(actual, expected);
// }

// TEST_F(Transform_unit_tests, Forward_RotatedTransform_ReturnsCorrectAxis)
// {
//     auto rot = Quaternion::FromEulerAngles(math::DegreesToRadians(90.0f), 0.0f, 0.0f);
//     auto* t = registry.Emplace(TestHandle, Vector3::Zero(), rot, Vector3::One(), EntityHandle::Null());
//     auto actual = t->Forward();
//     auto expected = Vector3::Down();
//     EXPECT_EQ(actual, expected);
// }

// TEST_F(Transform_unit_tests, Forward_RotatedParent_ChildReturnsCorrectAxis)
// {
//     auto rot = Quaternion::FromEulerAngles(math::DegreesToRadians(90.0f), 0.0f, 0.0f);
//     registry.Emplace(TestHandle, Vector3::Zero(), rot, Vector3::One(), EntityHandle::Null());
//     auto child = registry.Emplace(TestHandle2, Vector3::Zero(), Quaternion::Identity(), Vector3::One(), TestHandle);
//     auto actual = child->Forward();
//     auto expected = Vector3::Down();
//     EXPECT_EQ(actual, expected);
// }

// TEST_F(Transform_unit_tests, Right_ReturnsNormalizedVector)
// {
//     auto* t = registry.Emplace(TestHandle, TestPos1, TestRotQuat2, TestScale2, EntityHandle::Null());
//     auto actual = t->Right();
//     auto expected = Normalize(actual);
//     EXPECT_EQ(actual, expected);
// }

// TEST_F(Transform_unit_tests, Right_RotatedTransform_ReturnsCorrectAxis)
// {
//     auto rot = Quaternion::FromEulerAngles(math::DegreesToRadians(90.0f), 0.0f, 0.0f);
//     auto* t = registry.Emplace(TestHandle, Vector3::Zero(), rot, Vector3::One(), EntityHandle::Null());
//     auto actual = t->Right();
//     auto expected = Vector3::Right();
//     EXPECT_EQ(actual, expected);
// }

// TEST_F(Transform_unit_tests, Right_RotatedParent_ChildReturnsCorrectAxis)
// {
//     auto rot = Quaternion::FromEulerAngles(math::DegreesToRadians(90.0f), 0.0f, 0.0f);
//     registry.Emplace(TestHandle, Vector3::Zero(), rot, Vector3::One(), EntityHandle::Null());
//     auto* child = registry.Emplace(TestHandle2, Vector3::Zero(), Quaternion::Identity(), Vector3::One(), TestHandle);
//     auto actual = child->Right();
//     auto expected = Vector3::Right();
//     EXPECT_EQ(actual, expected);
// }

// TEST_F(Transform_unit_tests, Set_CalledOnRoot_CorrectlyRecomposesBothMatrices)
// {
//     auto* t = registry.Emplace(TestHandle, TestPos1, TestRotQuat1, TestScale1, EntityHandle::Null());
//     t->Set(TestPos2, TestRotQuat2, TestScale2);
//     EXPECT_EQ(t->GetPosition(), TestPos2);
//     EXPECT_EQ(t->GetRotation(), TestRotQuat2);
//     EXPECT_EQ(t->GetScale(), TestScale2);
//     EXPECT_EQ(t->GetLocalPosition(), TestPos2);
//     EXPECT_EQ(t->GetLocalRotation(), TestRotQuat2);
//     EXPECT_EQ(t->GetLocalScale(), TestScale2);
// }

// TEST_F(Transform_unit_tests, Set_CalledOnParent_OnlyRecomposesChildWorldMatrix)
// {
//     registry.Emplace(TestHandle, TestPos1, TestRotQuat1, TestScale1, EntityHandle::Null());
//     auto parent = registry.GetTransformView(TestHandle);
//     auto* child = registry.Emplace(TestHandle2, Vector3::Zero(), Quaternion::Identity(), Vector3::One(), TestHandle);
//     parent->Set(TestPos2, TestRotQuat2, TestScale2);
//     EXPECT_EQ(child->GetPosition(), TestPos2);
//     EXPECT_EQ(child->GetRotation(), TestRotQuat2);
//     EXPECT_EQ(child->GetScale(), TestScale2);
//     EXPECT_EQ(child->GetLocalPosition(), Vector3::Zero());
//     EXPECT_EQ(child->GetLocalRotation(), Quaternion::Identity());
//     EXPECT_EQ(child->GetLocalScale(), Vector3::One());
// }

// TEST_F(Transform_unit_tests, SetAngleOverload_CalledOnRoot_CorrectlyRecomposesBothMatrices)
// {
//     auto* t = registry.Emplace(TestHandle, TestPos1, TestRotQuat1, TestScale1, EntityHandle::Null());
//     t->Set(TestPos2, TestRotAngles, TestScale2);
//     EXPECT_EQ(t->GetPosition(), TestPos2);
//     auto expectedRot = Quaternion::FromEulerAngles(TestRotAngles);
//     EXPECT_EQ(t->GetRotation(), expectedRot);
//     EXPECT_EQ(t->GetScale(), TestScale2);
//     EXPECT_EQ(t->GetLocalPosition(), TestPos2);
//     EXPECT_EQ(t->GetLocalRotation(), expectedRot);
//     EXPECT_EQ(t->GetLocalScale(), TestScale2);
// }

// TEST_F(Transform_unit_tests, SetAngleOverload_CalledOnParent_OnlyRecomposesChildWorldMatrix)
// {
//     registry.Emplace(TestHandle, TestPos1, TestRotQuat1, TestScale1, EntityHandle::Null());
//     registry.Emplace(TestHandle2, Vector3::Zero(), Quaternion::Identity(), Vector3::One(), TestHandle);
    
//     auto* parent = GetComponent<Transform>(TestHandle);
//     auto* child = GetComponent<Transform>(TestHandle2);
//     parent->Set(TestPos2, TestRotAngles, TestScale2);
//     EXPECT_EQ(child->GetPosition(), TestPos2);
//     auto expectedRot = Quaternion::FromEulerAngles(TestRotAngles);
//     EXPECT_EQ(child->GetRotation(), expectedRot);
//     EXPECT_EQ(child->GetScale(), TestScale2);
//     EXPECT_EQ(child->GetLocalPosition(), Vector3::Zero());
//     EXPECT_EQ(child->GetLocalRotation(), Quaternion::Identity());
//     EXPECT_EQ(child->GetLocalScale(), Vector3::One());
// }

// TEST_F(Transform_unit_tests, SetPosition_CalledOnRoot_OnlyPositionModified)
// {
//     auto* t = registry.Emplace(TestHandle, TestPos1, TestRotQuat1, TestScale1, EntityHandle::Null());
//     t.SetPosition(TestPos2);
//     EXPECT_EQ(t.GetPosition(), TestPos2);
//     EXPECT_EQ(t.GetRotation(), TestRotQuat1);
//     EXPECT_EQ(t.GetScale(), TestScale1);
//     EXPECT_EQ(t.GetLocalPosition(), TestPos2);
//     EXPECT_EQ(t.GetLocalRotation(), TestRotQuat1);
//     EXPECT_EQ(t.GetLocalScale(), TestScale1);
// }

// TEST_F(Transform_unit_tests, SetPosition_CalledOnParent_OnlyWorldPositionModifiedInChild)
// {
//     auto parent = registry.Emplace(TestHandle, TestPos1, TestRotQuat1, TestScale1, EntityHandle::Null());
//     auto child = registry.Emplace(TestHandle2, Vector3::Zero(), Quaternion::Identity(), Vector3::One(), TestHandle);
//     parent.SetPosition(TestPos2);
//     EXPECT_EQ(child.GetPosition(), TestPos2);
//     EXPECT_EQ(child.GetRotation(), TestRotQuat1);
//     EXPECT_EQ(child.GetScale(), TestScale1);
//     EXPECT_EQ(child.GetLocalPosition(), Vector3::Zero());
//     EXPECT_EQ(child.GetLocalRotation(), Quaternion::Identity());
//     EXPECT_EQ(child.GetLocalScale(), Vector3::One());
// }

// TEST_F(Transform_unit_tests, SetRotation_CalledOnRoot_OnlyRotationModified)
// {
//     auto* t = registry.Emplace(TestHandle, TestPos1, TestRotQuat1, TestScale1, EntityHandle::Null());
//     t.SetRotation(TestRotQuat2);
//     EXPECT_EQ(t.GetPosition(), TestPos1);
//     EXPECT_EQ(t.GetRotation(), TestRotQuat2);
//     EXPECT_EQ(t.GetScale(), TestScale1);
//     EXPECT_EQ(t.GetLocalPosition(), TestPos1);
//     EXPECT_EQ(t.GetLocalRotation(), TestRotQuat2);
//     EXPECT_EQ(t.GetLocalScale(), TestScale1);
// }

// TEST_F(Transform_unit_tests, SetRotation_CalledOnParent_OnlyWorldRotationModifiedInChild)
// {
//     auto parent = registry.Emplace(TestHandle, TestPos1, TestRotQuat1, TestScale1, EntityHandle::Null());
//     auto child = registry.Emplace(TestHandle2, Vector3::Zero(), Quaternion::Identity(), Vector3::One(), TestHandle);
//     parent.SetRotation(TestRotQuat2);
//     EXPECT_EQ(child.GetPosition(), TestPos1);
//     EXPECT_EQ(child.GetRotation(), TestRotQuat2);
//     EXPECT_EQ(child.GetScale(), TestScale1);
//     EXPECT_EQ(child.GetLocalPosition(), Vector3::Zero());
//     EXPECT_EQ(child.GetLocalRotation(), Quaternion::Identity());
//     EXPECT_EQ(child.GetLocalScale(), Vector3::One());
// }

// TEST_F(Transform_unit_tests, SetRotationAnglesOverload_CalledOnRoot_OnlyRotationModified)
// {
//     auto* t = registry.Emplace(TestHandle, TestPos1, TestRotQuat1, TestScale1, EntityHandle::Null());
//     t.SetRotation(TestRotAngles);
//     EXPECT_EQ(t.GetPosition(), TestPos1);
//     auto expectedRot = Quaternion::FromEulerAngles(TestRotAngles);
//     EXPECT_EQ(t.GetRotation(), expectedRot);
//     EXPECT_EQ(t.GetScale(), TestScale1);
//     EXPECT_EQ(t.GetLocalPosition(), TestPos1);
//     EXPECT_EQ(t.GetLocalRotation(), expectedRot);
//     EXPECT_EQ(t.GetLocalScale(), TestScale1);
// }

// TEST_F(Transform_unit_tests, SetRotationAnglesOverload_CalledOnParent_OnlyWorldRotationModifiedInChild)
// {
//     auto parent = registry.Emplace(TestHandle, TestPos1, TestRotQuat1, TestScale1, EntityHandle::Null());
//     auto child = registry.Emplace(TestHandle2, Vector3::Zero(), Quaternion::Identity(), Vector3::One(), TestHandle);
//     parent.SetRotation(TestRotAngles);
//     EXPECT_EQ(child.GetPosition(), TestPos1);
//     auto expectedRot = Quaternion::FromEulerAngles(TestRotAngles);
//     EXPECT_EQ(child.GetRotation(), expectedRot);
//     EXPECT_EQ(child.GetScale(), TestScale1);
//     EXPECT_EQ(child.GetLocalPosition(), Vector3::Zero());
//     EXPECT_EQ(child.GetLocalRotation(), Quaternion::Identity());
//     EXPECT_EQ(child.GetLocalScale(), Vector3::One());
// }

// TEST_F(Transform_unit_tests, SetScale_CalledOnRoot_OnlyScaleModified)
// {
//     auto* t = registry.Emplace(TestHandle, TestPos1, TestRotQuat1, TestScale1, EntityHandle::Null());
//     t.SetScale(TestScale2);
//     EXPECT_EQ(t.GetPosition(), TestPos1);
//     EXPECT_EQ(t.GetRotation(), TestRotQuat1);
//     EXPECT_EQ(t.GetScale(), TestScale2);
//     EXPECT_EQ(t.GetLocalPosition(), TestPos1);
//     EXPECT_EQ(t.GetLocalRotation(), TestRotQuat1);
//     EXPECT_EQ(t.GetLocalScale(), TestScale2);
// }

// TEST_F(Transform_unit_tests, SetScale_CalledOnParent_OnlyWorldScaleModifiedInChild)
// {
//     auto parent = registry.Emplace(TestHandle, TestPos1, TestRotQuat1, TestScale1, EntityHandle::Null());
//     auto child = registry.Emplace(TestHandle2, Vector3::Zero(), Quaternion::Identity(), Vector3::One(), TestHandle);
//     parent.SetScale(TestScale2);
//     EXPECT_EQ(child.GetPosition(), TestPos1);
//     EXPECT_EQ(child.GetRotation(), TestRotQuat1);
//     EXPECT_EQ(child.GetScale(), TestScale2);
//     EXPECT_EQ(child.GetLocalPosition(), Vector3::Zero());
//     EXPECT_EQ(child.GetLocalRotation(), Quaternion::Identity());
//     EXPECT_EQ(child.GetLocalScale(), Vector3::One());
// }

// TEST_F(Transform_unit_tests, Translate_CalledOnRoot_OnlyPositionModified)
// {
//     auto* t = registry.Emplace(TestHandle, TestPos1, TestRotQuat1, TestScale1, EntityHandle::Null());
//     t.Translate(TestPos1);
//     auto expectedPos = TestPos1 + TestPos1;
//     EXPECT_EQ(t.GetPosition(), expectedPos);
//     EXPECT_EQ(t.GetRotation(), TestRotQuat1);
//     EXPECT_EQ(t.GetScale(), TestScale1);
//     EXPECT_EQ(t.GetLocalPosition(), expectedPos);
//     EXPECT_EQ(t.GetLocalRotation(), TestRotQuat1);
//     EXPECT_EQ(t.GetLocalScale(), TestScale1);
// }

// TEST_F(Transform_unit_tests, Translate_CalledOnParent_OnlyWorldPositionModifiedInChild)
// {
//     auto parent = registry.Emplace(TestHandle, TestPos1, TestRotQuat1, TestScale1, EntityHandle::Null());
//     auto child = registry.Emplace(TestHandle2, Vector3::Zero(), Quaternion::Identity(), Vector3::One(), TestHandle);
//     parent.Translate(TestPos1);
//     auto expectedPos = TestPos1 + TestPos1;
//     EXPECT_EQ(child.GetPosition(), expectedPos);
//     EXPECT_EQ(child.GetRotation(), TestRotQuat1);
//     EXPECT_EQ(child.GetScale(), TestScale1);
//     EXPECT_EQ(child.GetLocalPosition(), Vector3::Zero());
//     EXPECT_EQ(child.GetLocalRotation(), Quaternion::Identity());
//     EXPECT_EQ(child.GetLocalScale(), Vector3::One());
// }

// TEST_F(Transform_unit_tests, TranslateLocalSpace_CalledOnRoot_OnlyPositionModified)
// {
//     auto* t = registry.Emplace(TestHandle, TestPos1, Quaternion::Identity(), TestScale1, EntityHandle::Null());
//     t.TranslateLocalSpace(TestPos1);
//     auto expectedPos = TestPos1 + TestPos1;
//     EXPECT_EQ(t.GetPosition(), expectedPos);
//     EXPECT_EQ(t.GetRotation(), Quaternion::Identity());
//     EXPECT_EQ(t.GetScale(), TestScale1);
//     EXPECT_EQ(t.GetLocalPosition(), expectedPos);
//     EXPECT_EQ(t.GetLocalRotation(), Quaternion::Identity());
//     EXPECT_EQ(t.GetLocalScale(), TestScale1);
// }

// TEST_F(Transform_unit_tests, TranslateLocalSpace_CalledOnParent_OnlyWorldPositionModifiedInChild)
// {
//     auto parent = registry.Emplace(TestHandle, TestPos1, Quaternion::Identity(), TestScale1, EntityHandle::Null());
//     auto child = registry.Emplace(TestHandle2, Vector3::Zero(), Quaternion::Identity(), Vector3::One(), TestHandle);
//     parent.TranslateLocalSpace(TestPos1);
//     auto expectedPos = TestPos1 + TestPos1;
//     EXPECT_EQ(child.GetPosition(), expectedPos);
//     EXPECT_EQ(child.GetRotation(), Quaternion::Identity());
//     EXPECT_EQ(child.GetScale(), TestScale1);
//     EXPECT_EQ(child.GetLocalPosition(), Vector3::Zero());
//     EXPECT_EQ(child.GetLocalRotation(), Quaternion::Identity());
//     EXPECT_EQ(child.GetLocalScale(), Vector3::One());
// }

// TEST_F(Transform_unit_tests, Rotate_CalledOnRoot_OnlyRotationModified)
// {
//     auto* t = registry.Emplace(TestHandle, TestPos1, Quaternion::Identity(), TestScale1, EntityHandle::Null());
//     t.Rotate(TestRotQuat2);
//     EXPECT_EQ(t.GetPosition(), TestPos1);
//     EXPECT_EQ(t.GetRotation(), TestRotQuat2);
//     EXPECT_EQ(t.GetScale(), TestScale1);
//     EXPECT_EQ(t.GetLocalPosition(), TestPos1);
//     EXPECT_EQ(t.GetLocalRotation(), TestRotQuat2);
//     EXPECT_EQ(t.GetLocalScale(), TestScale1);
// }

// TEST_F(Transform_unit_tests, Rotate_CalledOnParent_OnlyWorldRotationModifiedInChild)
// {
//     auto parent = registry.Emplace(TestHandle, TestPos1, Quaternion::Identity(), TestScale1, EntityHandle::Null());
//     auto child = registry.Emplace(TestHandle2, Vector3::Zero(), Quaternion::Identity(), Vector3::One(), TestHandle);
//     parent.Rotate(TestRotQuat2);
//     EXPECT_EQ(child.GetPosition(), TestPos1);
//     EXPECT_EQ(child.GetRotation(), TestRotQuat2);
//     EXPECT_EQ(child.GetScale(), TestScale1);
//     EXPECT_EQ(child.GetLocalPosition(), Vector3::Zero());
//     EXPECT_EQ(child.GetLocalRotation(), Quaternion::Identity());
//     EXPECT_EQ(child.GetLocalScale(), Vector3::One());
// }

// TEST_F(Transform_unit_tests, RotateAxisAngleOverload_CalledOnRoot_OnlyRotationModified)
// {
//     auto* t = registry.Emplace(TestHandle, TestPos1, Quaternion::Identity(), TestScale1, EntityHandle::Null());
//     auto expectedAxis = Vector3::Up();
//     auto expectedAngle = 2.2f; 
//     t.Rotate(expectedAxis, expectedAngle);
//     auto actualQuat = t.GetRotation();
//     auto expectedQuat = Quaternion::FromAxisAngle(expectedAxis, expectedAngle);
//     EXPECT_EQ(t.GetPosition(), TestPos1);
//     EXPECT_EQ(actualQuat, expectedQuat);
//     EXPECT_EQ(t.GetScale(), TestScale1);
//     EXPECT_EQ(t.GetLocalPosition(), TestPos1);
//     EXPECT_EQ(t.GetLocalRotation(), expectedQuat);
//     EXPECT_EQ(t.GetLocalScale(), TestScale1);
// }

// TEST_F(Transform_unit_tests, RotateAxisAngleOverload_CalledOnParent_OnlyWorldRotationModifiedInChild)
// {
//     auto parent = registry.Emplace(TestHandle, TestPos1, Quaternion::Identity(), TestScale1, EntityHandle::Null());
//     auto* child = registry.Emplace(TestHandle2, Vector3::Zero(), Quaternion::Identity(), Vector3::One(), TestHandle);
//     auto expectedAxis = Vector3::Up();
//     auto expectedAngle = 2.2f; 
//     parent.Rotate(expectedAxis, expectedAngle);
//     EXPECT_EQ(child.GetPosition(), TestPos1);
//     auto expectedQuat = Quaternion::FromAxisAngle(expectedAxis, expectedAngle);
//     auto actualQuat = child.GetRotation();
//     EXPECT_EQ(actualQuat, expectedQuat);
//     EXPECT_EQ(child.GetScale(), TestScale1);
//     EXPECT_EQ(child.GetLocalPosition(), Vector3::Zero());
//     EXPECT_EQ(child.GetLocalRotation(), Quaternion::Identity());
//     EXPECT_EQ(child.GetLocalScale(), Vector3::One());
// }

// TEST_F(Transform_unit_tests, GetChildren_ChildConstructor_AddsChildToParent)
// {
//     auto parentHandle = TestHandle;
//     auto childHandle = TestHandle2;
//     auto parent = registry.Emplace(parentHandle, TestPos1, Quaternion::Identity(), TestScale1, EntityHandle::Null());
//     auto* child = registry.Emplace(childHandle, Vector3::Zero(), Quaternion::Identity(), Vector3::One(), parentHandle};
//     auto children = parent.GetChildren();
//     auto actualCount = children.size();
//     ASSERT_EQ(actualCount, 1u);
//     EXPECT_EQ(children[0], childHandle);
// }

// TEST_F(Transform_unit_tests, GetRoot_CalledOnRoot_ReturnsSelf)
// {
//     auto root = registry.Emplace(TestHandle, Vector3::Zero(), Quaternion::Identity(), Vector3::One(), EntityHandle::Null());
//     auto actual = root.GetRoot();
//     EXPECT_EQ(actual, TestHandle);
// }

// TEST_F(Transform_unit_tests, GetRoot_WithOneDepthLevel_ReturnsParent)
// {
//     auto parentHandle = TestHandle;
//     auto childHandle = TestHandle2;
//     auto parent = registry.Emplace(parentHandle, Vector3::Zero(), Quaternion::Identity(), Vector3::One(), EntityHandle::Null());
//     auto child = registry.Emplace(childHandle, Vector3::Zero(), Quaternion::Identity(), Vector3::One(), parentHandle};
//     auto actual = child.GetRoot();
//     EXPECT_EQ(actual, parentHandle);
// }

// TEST_F(Transform_unit_tests, GetRoot_WithTwoDepthLevels_ReturnsParentsParent)
// {
//     auto rootHandle = TestHandle;
//     auto parentHandle = TestHandle2;
//     auto childHandle = TestHandle3;
//     auto root = registry.Emplace(rootHandle, Vector3::Zero(), Quaternion::Identity(), Vector3::One(), EntityHandle::Null());
//     auto parent = registry.Emplace(parentHandle, Vector3::Zero(), Quaternion::Identity(), Vector3::One(), rootHandle};
//     auto child = registry.Emplace(childHandle, Vector3::Zero(), Quaternion::Identity(), Vector3::One(), parentHandle};
//     auto actual = child.GetRoot();
//     EXPECT_EQ(actual, rootHandle);
// }

// TEST_F(Transform_unit_tests, SetParent_RootChangedToChild_ParentAndChildUpdated)
// {
//     auto parentHandle = TestHandle;
//     auto childHandle = TestHandle2;
//     auto parent = registry.Emplace(parentHandle, Vector3::Zero(), Quaternion::Identity(), Vector3::One(), EntityHandle::Null());
//     auto child = registry.Emplace(childHandle, Vector3::Zero(), Quaternion::Identity(), Vector3::One(), EntityHandle::Null());
//     child.SetParent(parentHandle);
//     EXPECT_EQ(child.GetParent(), parentHandle);
//     auto children = parent.GetChildren();
//     ASSERT_EQ(children.size(), 1u);
//     EXPECT_EQ(children[0], childHandle);
// }

// TEST_F(Transform_unit_tests, SetParent_ChildReParented_ParentAndChildUpdated)
// {
//     auto initialParentHandle = TestHandle;
//     auto newParentHandle = TestHandle2;
//     auto childHandle = TestHandle3;
//     auto initialParent = registry.Emplace(initialParentHandle, Vector3::Zero(), Quaternion::Identity(), Vector3::One(), EntityHandle::Null());
//     auto newParent = registry.Emplace(newParentHandle, Vector3::Zero(), Quaternion::Identity(), Vector3::One(), EntityHandle::Null());
//     auto child = registry.Emplace(childHandle, Vector3::Zero(), Quaternion::Identity(), Vector3::One(), initialParentHandle};
//     child.SetParent(newParentHandle);
//     EXPECT_EQ(child.GetParent(), newParentHandle);
//     auto initialParentChildren = initialParent.GetChildren();
//     EXPECT_EQ(initialParentChildren.size(), 0u);
//     auto newParentChildren = newParent.GetChildren();
//     ASSERT_EQ(newParentChildren.size(), 1u);
//     EXPECT_EQ(newParentChildren[0], childHandle);
// }

// TEST_F(Transform_unit_tests, SetParent_ChildDetached_ParentAndChildUpdated)
// {
//     auto parentHandle = TestHandle;
//     auto childHandle = TestHandle2;
//     auto parent = registry.Emplace(parentHandle, Vector3::Zero(), Quaternion::Identity(), Vector3::One(), EntityHandle::Null());
//     auto child = registry.Emplace(childHandle, Vector3::Zero(), Quaternion::Identity(), Vector3::One(), parentHandle};
//     child.SetParent(EntityHandle::Null());
//     EXPECT_EQ(child.GetParent(), EntityHandle::Null());
//     auto children = parent.GetChildren();
//     EXPECT_EQ(children.size(), 0u);
// }

int main(int argc, char ** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}