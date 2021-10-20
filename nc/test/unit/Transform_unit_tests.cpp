#include "gtest/gtest.h"
#include "component/Transform.h"
#include "component/AutoComponentGroup.h"
#include "Ecs.h"

#include <iostream>

using namespace nc;

namespace nc
{
    registry_type* g_registry = nullptr;

    registry_type* ActiveRegistry()
    {
        return g_registry;
    }

    AutoComponentGroup::AutoComponentGroup(Entity) : m_components{} {}
    void AutoComponentGroup::SendOnDestroy() {}
}

constexpr auto TestLayer = Entity::layer_type{0u};
constexpr auto TestFlags = Entity::Flags::None;
constexpr auto TestHandle = Entity{1u, TestLayer, TestFlags};
constexpr auto TestHandle2 = Entity{2u, TestLayer, TestFlags};
constexpr auto TestHandle3 = Entity{3u, TestLayer, TestFlags};
constexpr auto TestPos1 = Vector3{0.0f, 1.0f, 2.0f};
constexpr auto TestPos2 = Vector3{1.5f, 1.5f, 1.5f};
constexpr auto TestScale1 = Vector3{1.0f, 3.0f, 1.0f};
constexpr auto TestScale2 = Vector3{10.0f, 1.0f, 3.7f};
constexpr auto TestScale3 = Vector3::Splat(2.0f);
constexpr auto TestRotAngles = Vector3{0.15f, 1.0f, 0.177f};
const auto TestRotQuat1 = Quaternion::FromEulerAngles(0.0f, 0.0f, 90.0f);
const auto TestRotQuat2 = Quaternion::FromEulerAngles(1.5f, 1.5f, 1.5f);
const auto TestRotQuat3 = Quaternion::FromEulerAngles(2.5, 0.0f, 1.0f);

class Transform_unit_tests : public ::testing::Test
{
    public:
        std::unique_ptr<registry_type> registry;

    protected:
        void SetUp() override
        {
            registry = std::make_unique<registry_type>(100u);
            g_registry = registry.get();
        }

        void TearDown() override
        {
            registry->Clear();
            g_registry = nullptr;
            registry = nullptr;
        }
};

TEST_F(Transform_unit_tests, GetLocalPosition_CalledFromRoot_DecomposesPositionFromMatrix)
{
    auto e = registry->Add<Entity>(EntityInfo{.position = TestPos1});
    auto* t = registry->Get<Transform>(e);
    auto actual = t->GetLocalPosition();
    EXPECT_EQ(actual, TestPos1);
}

TEST_F(Transform_unit_tests, GetLocalPosition_CalledFromChild_ResultIsRelativeToParent)
{
    auto parent = registry->Add<Entity>(EntityInfo{.position = TestPos1});
    auto child = registry->Add<Entity>(EntityInfo{.position = Vector3::Zero(), .parent = parent});
    auto actual = registry->Get<Transform>(child)->GetLocalPosition();
    EXPECT_EQ(actual, Vector3::Zero());
}

TEST_F(Transform_unit_tests, GetPosition_CalledFromRoot_DecomposesPositionFromMatrix)
{
    auto e = registry->Add<Entity>({.position = TestPos1});
    auto t = registry->Get<Transform>(e);
    auto actual = t->GetPosition();
    EXPECT_EQ(actual, TestPos1);
}

TEST_F(Transform_unit_tests, GetPosition_CalledFromChild_ResultIncludesParentPosition)
{
    auto parent = registry->Add<Entity>(EntityInfo{.position = TestPos1});
    auto child = registry->Add<Entity>(EntityInfo{.position = TestPos2, .parent = parent});
    auto t = registry->Get<Transform>(child);
    auto actual = t->GetPosition();
    auto expected = TestPos1 + TestPos2;
    EXPECT_EQ(actual, expected);
}

TEST_F(Transform_unit_tests, GetLocalRotation_CalledFromRoot_DecomposesRotationFromMatrix)
{
    auto e = registry->Add<Entity>(EntityInfo{.rotation = TestRotQuat1});
    auto t = registry->Get<Transform>(e);
    auto actual = t->GetLocalRotation();
    EXPECT_EQ(actual, TestRotQuat1);
}

TEST_F(Transform_unit_tests, GetLocalRotation_CalledFromChild_ResultIsRelativeToParent)
{
    auto parent = registry->Add<Entity>(EntityInfo{.rotation = TestRotQuat2});
    auto child = registry->Add<Entity>(EntityInfo{.rotation = TestRotQuat2, .parent = parent});
    auto t = registry->Get<Transform>(child);
    auto actual = t->GetLocalRotation();
    EXPECT_EQ(actual, TestRotQuat2);
}

TEST_F(Transform_unit_tests, GetRotation_CalledFromRoot_DecomposesRotationFromMatrix)
{
    auto e = registry->Add<Entity>(EntityInfo{.rotation = TestRotQuat2});
    auto t = registry->Get<Transform>(e);
    auto actual = t->GetRotation();
    EXPECT_EQ(actual, TestRotQuat2);
}

TEST_F(Transform_unit_tests, GetRotation_CalledFromChild_ResultIncludesParentRotation)
{
    auto parent = registry->Add<Entity>(EntityInfo{.rotation = TestRotQuat2});
    auto child = registry->Add<Entity>(EntityInfo{.rotation = TestRotQuat3, .parent = parent});
    auto t = registry->Get<Transform>(child);
    auto actual = t->GetRotation();
    auto expected = Multiply(TestRotQuat2, TestRotQuat3);
    EXPECT_EQ(actual, expected);
}

TEST_F(Transform_unit_tests, GetLocalScale_CalledFromRoot_DecomposesScaleFromMatrix)
{
    auto e = registry->Add<Entity>(EntityInfo{.scale = TestScale1});
    auto t = registry->Get<Transform>(e);
    auto actual = t->GetLocalScale();
    EXPECT_EQ(actual, TestScale1);
}

TEST_F(Transform_unit_tests, GetLocalScale_CalledFromChild_ResultIsRelativeToParent)
{
    auto parent = registry->Add<Entity>(EntityInfo{.scale = TestScale2});
    auto child = registry->Add<Entity>(EntityInfo{.scale = TestScale3, .parent = parent});
    auto t = registry->Get<Transform>(child);
    auto actual = t->GetLocalScale();
    EXPECT_EQ(actual, TestScale3);
}

TEST_F(Transform_unit_tests, GetScale_CalledFromRoot_DecomposesScaleFromMatrix)
{
    auto e = registry->Add<Entity>(EntityInfo{.scale = TestScale2});
    auto t = registry->Get<Transform>(e);
    auto actual = t->GetScale();
    EXPECT_EQ(actual, TestScale2);
}

TEST_F(Transform_unit_tests, GetScale_CalledFromChild_ResultIncludesParentScale)
{
    auto parent = registry->Add<Entity>(EntityInfo{.scale = TestScale2});
    auto child = registry->Add<Entity>(EntityInfo{.scale = TestScale3, .parent = parent});
    auto t = registry->Get<Transform>(child);
    auto actual = t->GetScale();
    auto expected = HadamardProduct(TestScale2, TestScale3);
    EXPECT_EQ(actual, expected);
}

TEST_F(Transform_unit_tests, Up_ReturnsNormalizedVector)
{
    auto e = registry->Add<Entity>(EntityInfo{.rotation = TestRotQuat2, .scale = TestScale2});
    auto t = registry->Get<Transform>(e);
    auto actual = t->Up();
    auto expected = Normalize(actual);
    EXPECT_EQ(actual, expected);
}

TEST_F(Transform_unit_tests, Up_RotatedTransform_ReturnsCorrectAxis)
{
    auto rot = Quaternion::FromEulerAngles(math::DegreesToRadians(90.0f), 0.0f, 0.0f);
    auto e = registry->Add<Entity>(EntityInfo{.rotation = rot});
    auto t = registry->Get<Transform>(e);
    auto actual = t->Up();
    auto expected = Vector3::Front();
    EXPECT_EQ(actual, expected);
}

TEST_F(Transform_unit_tests, Up_RotatedParent_ChildReturnsCorrectAxis)
{
    auto rot = Quaternion::FromEulerAngles(math::DegreesToRadians(90.0f), 0.0f, 0.0f);
    auto parent = registry->Add<Entity>(EntityInfo{.rotation = rot});
    auto child = registry->Add<Entity>(EntityInfo{.rotation = Quaternion::Identity(), .parent = parent});
    auto t = registry->Get<Transform>(child);
    auto actual = t->Up();
    auto expected = Vector3::Front();
    EXPECT_EQ(actual, expected);
}

TEST_F(Transform_unit_tests, Forward_ReturnsNormalizedVector)
{
    auto e = registry->Add<Entity>(EntityInfo{.rotation = TestRotQuat2, .scale = TestScale2});
    auto t = registry->Get<Transform>(e);
    auto actual = t->Forward();
    auto expected = Normalize(actual);
    EXPECT_EQ(actual, expected);
}

TEST_F(Transform_unit_tests, Forward_RotatedTransform_ReturnsCorrectAxis)
{
    auto rot = Quaternion::FromEulerAngles(math::DegreesToRadians(90.0f), 0.0f, 0.0f);
    auto e = registry->Add<Entity>(EntityInfo{.rotation = rot});
    auto t = registry->Get<Transform>(e);
    auto actual = t->Forward();
    auto expected = Vector3::Down();
    EXPECT_EQ(actual, expected);
}

TEST_F(Transform_unit_tests, Forward_RotatedParent_ChildReturnsCorrectAxis)
{
    auto rot = Quaternion::FromEulerAngles(math::DegreesToRadians(90.0f), 0.0f, 0.0f);
    auto parent = registry->Add<Entity>(EntityInfo{.rotation = rot});
    auto child = registry->Add<Entity>(EntityInfo{.rotation = Quaternion::Identity(), .parent = parent});
    auto t = registry->Get<Transform>(child);
    auto actual = t->Forward();
    auto expected = Vector3::Down();
    EXPECT_EQ(actual, expected);
}

TEST_F(Transform_unit_tests, Right_ReturnsNormalizedVector)
{
    auto e = registry->Add<Entity>(EntityInfo{.rotation = TestRotQuat2, .scale = TestScale2});
    auto t = registry->Get<Transform>(e);
    auto actual = t->Right();
    auto expected = Normalize(actual);
    EXPECT_EQ(actual, expected);
}

TEST_F(Transform_unit_tests, Right_RotatedTransform_ReturnsCorrectAxis)
{
    auto rot = Quaternion::FromEulerAngles(math::DegreesToRadians(90.0f), 0.0f, 0.0f);
    auto e = registry->Add<Entity>(EntityInfo{.rotation = rot});
    auto t = registry->Get<Transform>(e);
    auto actual = t->Right();
    auto expected = Vector3::Right();
    EXPECT_EQ(actual, expected);
}

TEST_F(Transform_unit_tests, Right_RotatedParent_ChildReturnsCorrectAxis)
{
    auto rot = Quaternion::FromEulerAngles(math::DegreesToRadians(90.0f), 0.0f, 0.0f);
    auto parent = registry->Add<Entity>(EntityInfo{.rotation = rot});
    auto child = registry->Add<Entity>(EntityInfo{.rotation = Quaternion::Identity(), .parent = parent});
    auto t = registry->Get<Transform>(child);
    auto actual = t->Right();
    auto expected = Vector3::Right();
    EXPECT_EQ(actual, expected);
}

TEST_F(Transform_unit_tests, Set_CalledOnRoot_CorrectlyRecomposesBothMatrices)
{
    auto e = registry->Add<Entity>(EntityInfo{.position = TestPos1, .rotation = TestRotQuat1, .scale = TestScale1});
    auto t = registry->Get<Transform>(e);
    t->Set(TestPos2, TestRotQuat2, TestScale2);
    EXPECT_EQ(t->GetPosition(), TestPos2);
    EXPECT_EQ(t->GetRotation(), TestRotQuat2);
    EXPECT_EQ(t->GetScale(), TestScale2);
    EXPECT_EQ(t->GetLocalPosition(), TestPos2);
    EXPECT_EQ(t->GetLocalRotation(), TestRotQuat2);
    EXPECT_EQ(t->GetLocalScale(), TestScale2);
}

TEST_F(Transform_unit_tests, Set_CalledOnParent_OnlyRecomposesChildWorldMatrix)
{
    auto parent = registry->Add<Entity>(EntityInfo{.position = TestPos1, .rotation = TestRotQuat1, .scale = TestScale1});
    auto child = registry->Add<Entity>(EntityInfo{.parent = parent});
    auto tParent = registry->Get<Transform>(parent);
    tParent->Set(TestPos2, TestRotQuat2, TestScale2);
    auto tChild = registry->Get<Transform>(child);
    EXPECT_EQ(tChild->GetPosition(), TestPos2);
    EXPECT_EQ(tChild->GetRotation(), TestRotQuat2);
    EXPECT_EQ(tChild->GetScale(), TestScale2);
    EXPECT_EQ(tChild->GetLocalPosition(), Vector3::Zero());
    EXPECT_EQ(tChild->GetLocalRotation(), Quaternion::Identity());
    EXPECT_EQ(tChild->GetLocalScale(), Vector3::One());
}

TEST_F(Transform_unit_tests, SetAngleOverload_CalledOnRoot_CorrectlyRecomposesBothMatrices)
{
    auto e = registry->Add<Entity>(EntityInfo{.position = TestPos1, .rotation = TestRotQuat1, .scale = TestScale1});
    auto t = registry->Get<Transform>(e);
    t->Set(TestPos2, TestRotAngles, TestScale2);
    EXPECT_EQ(t->GetPosition(), TestPos2);
    auto expectedRot = Quaternion::FromEulerAngles(TestRotAngles);
    EXPECT_EQ(t->GetRotation(), expectedRot);
    EXPECT_EQ(t->GetScale(), TestScale2);
    EXPECT_EQ(t->GetLocalPosition(), TestPos2);
    EXPECT_EQ(t->GetLocalRotation(), expectedRot);
    EXPECT_EQ(t->GetLocalScale(), TestScale2);
}

TEST_F(Transform_unit_tests, SetAngleOverload_CalledOnParent_OnlyRecomposesChildWorldMatrix)
{
    auto parent = registry->Add<Entity>(EntityInfo{.position = TestPos1, .rotation = TestRotQuat1, .scale = TestScale1});
    auto child = registry->Add<Entity>(EntityInfo{.parent = parent});
    auto* tParent = registry->Get<Transform>(parent);
    auto* tChild = registry->Get<Transform>(child);
    tParent->Set(TestPos2, TestRotAngles, TestScale2);
    EXPECT_EQ(tChild->GetPosition(), TestPos2);
    auto expectedRot = Quaternion::FromEulerAngles(TestRotAngles);
    EXPECT_EQ(tChild->GetRotation(), expectedRot);
    EXPECT_EQ(tChild->GetScale(), TestScale2);
    EXPECT_EQ(tChild->GetLocalPosition(), Vector3::Zero());
    EXPECT_EQ(tChild->GetLocalRotation(), Quaternion::Identity());
    EXPECT_EQ(tChild->GetLocalScale(), Vector3::One());
}

TEST_F(Transform_unit_tests, SetPosition_CalledOnRoot_OnlyPositionModified)
{
    auto e = registry->Add<Entity>(EntityInfo{.position = TestPos1, .rotation = TestRotQuat1, .scale = TestScale1});
    auto t = registry->Get<Transform>(e);
    t->SetPosition(TestPos2);
    EXPECT_EQ(t->GetPosition(), TestPos2);
    EXPECT_EQ(t->GetRotation(), TestRotQuat1);
    EXPECT_EQ(t->GetScale(), TestScale1);
    EXPECT_EQ(t->GetLocalPosition(), TestPos2);
    EXPECT_EQ(t->GetLocalRotation(), TestRotQuat1);
    EXPECT_EQ(t->GetLocalScale(), TestScale1);
}

TEST_F(Transform_unit_tests, SetPosition_CalledOnParent_OnlyWorldPositionModifiedInChild)
{
    auto parent = registry->Add<Entity>(EntityInfo{.position = TestPos1, .rotation = TestRotQuat1, .scale = TestScale1});
    auto child = registry->Add<Entity>(EntityInfo{.parent = parent});
    auto tParent = registry->Get<Transform>(parent);
    auto tChild = registry->Get<Transform>(child);
    tParent->SetPosition(TestPos2);
    EXPECT_EQ(tChild->GetPosition(), TestPos2);
    EXPECT_EQ(tChild->GetRotation(), TestRotQuat1);
    EXPECT_EQ(tChild->GetScale(), TestScale1);
    EXPECT_EQ(tChild->GetLocalPosition(), Vector3::Zero());
    EXPECT_EQ(tChild->GetLocalRotation(), Quaternion::Identity());
    EXPECT_EQ(tChild->GetLocalScale(), Vector3::One());
}

TEST_F(Transform_unit_tests, SetRotation_CalledOnRoot_OnlyRotationModified)
{
    auto e = registry->Add<Entity>(EntityInfo{.position = TestPos1, .rotation = TestRotQuat1, .scale = TestScale1});
    auto t = registry->Get<Transform>(e);
    t->SetRotation(TestRotQuat2);
    EXPECT_EQ(t->GetPosition(), TestPos1);
    EXPECT_EQ(t->GetRotation(), TestRotQuat2);
    EXPECT_EQ(t->GetScale(), TestScale1);
    EXPECT_EQ(t->GetLocalPosition(), TestPos1);
    EXPECT_EQ(t->GetLocalRotation(), TestRotQuat2);
    EXPECT_EQ(t->GetLocalScale(), TestScale1);
}

TEST_F(Transform_unit_tests, SetRotation_CalledOnParent_OnlyWorldRotationModifiedInChild)
{
    auto parent = registry->Add<Entity>(EntityInfo{.position = TestPos1, .rotation = TestRotQuat1, .scale = TestScale1});
    auto child = registry->Add<Entity>(EntityInfo{.parent = parent});
    auto tParent = registry->Get<Transform>(parent);
    auto tChild = registry->Get<Transform>(child);
    tParent->SetRotation(TestRotQuat2);
    EXPECT_EQ(tChild->GetPosition(), TestPos1);
    EXPECT_EQ(tChild->GetRotation(), TestRotQuat2);
    EXPECT_EQ(tChild->GetScale(), TestScale1);
    EXPECT_EQ(tChild->GetLocalPosition(), Vector3::Zero());
    EXPECT_EQ(tChild->GetLocalRotation(), Quaternion::Identity());
    EXPECT_EQ(tChild->GetLocalScale(), Vector3::One());
}

TEST_F(Transform_unit_tests, SetRotationAnglesOverload_CalledOnRoot_OnlyRotationModified)
{
    auto e = registry->Add<Entity>(EntityInfo{.position = TestPos1, .rotation = TestRotQuat1, .scale = TestScale1});
    auto t = registry->Get<Transform>(e);
    t->SetRotation(TestRotAngles);
    EXPECT_EQ(t->GetPosition(), TestPos1);
    auto expectedRot = Quaternion::FromEulerAngles(TestRotAngles);
    EXPECT_EQ(t->GetRotation(), expectedRot);
    EXPECT_EQ(t->GetScale(), TestScale1);
    EXPECT_EQ(t->GetLocalPosition(), TestPos1);
    EXPECT_EQ(t->GetLocalRotation(), expectedRot);
    EXPECT_EQ(t->GetLocalScale(), TestScale1);
}

TEST_F(Transform_unit_tests, SetRotationAnglesOverload_CalledOnParent_OnlyWorldRotationModifiedInChild)
{
    auto parent = registry->Add<Entity>(EntityInfo{.position = TestPos1, .rotation = TestRotQuat1, .scale = TestScale1});
    auto child = registry->Add<Entity>(EntityInfo{.parent = parent});
    auto tParent = registry->Get<Transform>(parent);
    auto tChild = registry->Get<Transform>(child);
    tParent->SetRotation(TestRotAngles);
    EXPECT_EQ(tChild->GetPosition(), TestPos1);
    auto expectedRot = Quaternion::FromEulerAngles(TestRotAngles);
    EXPECT_EQ(tChild->GetRotation(), expectedRot);
    EXPECT_EQ(tChild->GetScale(), TestScale1);
    EXPECT_EQ(tChild->GetLocalPosition(), Vector3::Zero());
    EXPECT_EQ(tChild->GetLocalRotation(), Quaternion::Identity());
    EXPECT_EQ(tChild->GetLocalScale(), Vector3::One());
}

TEST_F(Transform_unit_tests, SetScale_CalledOnRoot_OnlyScaleModified)
{
    auto e = registry->Add<Entity>(EntityInfo{.position = TestPos1, .rotation = TestRotQuat1, .scale = TestScale1});
    auto t = registry->Get<Transform>(e);
    t->SetScale(TestScale2);
    EXPECT_EQ(t->GetPosition(), TestPos1);
    EXPECT_EQ(t->GetRotation(), TestRotQuat1);
    EXPECT_EQ(t->GetScale(), TestScale2);
    EXPECT_EQ(t->GetLocalPosition(), TestPos1);
    EXPECT_EQ(t->GetLocalRotation(), TestRotQuat1);
    EXPECT_EQ(t->GetLocalScale(), TestScale2);
}

TEST_F(Transform_unit_tests, SetScale_CalledOnParent_OnlyWorldScaleModifiedInChild)
{
    auto parent = registry->Add<Entity>(EntityInfo{.position = TestPos1, .rotation = TestRotQuat1, .scale = TestScale1});
    auto child = registry->Add<Entity>(EntityInfo{.parent = parent});
    auto tParent = registry->Get<Transform>(parent);
    auto tChild = registry->Get<Transform>(child);
    tParent->SetScale(TestScale2);
    EXPECT_EQ(tChild->GetPosition(), TestPos1);
    EXPECT_EQ(tChild->GetRotation(), TestRotQuat1);
    EXPECT_EQ(tChild->GetScale(), TestScale2);
    EXPECT_EQ(tChild->GetLocalPosition(), Vector3::Zero());
    EXPECT_EQ(tChild->GetLocalRotation(), Quaternion::Identity());
    EXPECT_EQ(tChild->GetLocalScale(), Vector3::One());
}

TEST_F(Transform_unit_tests, Translate_CalledOnRoot_OnlyPositionModified)
{
    auto e = registry->Add<Entity>(EntityInfo{.position = TestPos1, .rotation = TestRotQuat1, .scale = TestScale1});
    auto t = registry->Get<Transform>(e);
    t->Translate(TestPos1);
    auto expectedPos = TestPos1 + TestPos1;
    EXPECT_EQ(t->GetPosition(), expectedPos);
    EXPECT_EQ(t->GetRotation(), TestRotQuat1);
    EXPECT_EQ(t->GetScale(), TestScale1);
    EXPECT_EQ(t->GetLocalPosition(), expectedPos);
    EXPECT_EQ(t->GetLocalRotation(), TestRotQuat1);
    EXPECT_EQ(t->GetLocalScale(), TestScale1);
}

TEST_F(Transform_unit_tests, Translate_CalledOnParent_OnlyWorldPositionModifiedInChild)
{
    auto parent = registry->Add<Entity>(EntityInfo{.position = TestPos1, .rotation = TestRotQuat1, .scale = TestScale1});
    auto child = registry->Add<Entity>(EntityInfo{.parent = parent});
    auto tParent = registry->Get<Transform>(parent);
    auto tChild = registry->Get<Transform>(child);
    tParent->Translate(TestPos1);
    auto expectedPos = TestPos1 + TestPos1;
    EXPECT_EQ(tChild->GetPosition(), expectedPos);
    EXPECT_EQ(tChild->GetRotation(), TestRotQuat1);
    EXPECT_EQ(tChild->GetScale(), TestScale1);
    EXPECT_EQ(tChild->GetLocalPosition(), Vector3::Zero());
    EXPECT_EQ(tChild->GetLocalRotation(), Quaternion::Identity());
    EXPECT_EQ(tChild->GetLocalScale(), Vector3::One());
}

TEST_F(Transform_unit_tests, TranslateLocalSpace_CalledOnRoot_OnlyPositionModified)
{
    auto e = registry->Add<Entity>(EntityInfo{.position = TestPos1, .scale = TestScale1});
    auto t = registry->Get<Transform>(e);
    t->TranslateLocalSpace(TestPos1);
    auto expectedPos = TestPos1 + TestPos1;
    EXPECT_EQ(t->GetPosition(), expectedPos);
    EXPECT_EQ(t->GetRotation(), Quaternion::Identity());
    EXPECT_EQ(t->GetScale(), TestScale1);
    EXPECT_EQ(t->GetLocalPosition(), expectedPos);
    EXPECT_EQ(t->GetLocalRotation(), Quaternion::Identity());
    EXPECT_EQ(t->GetLocalScale(), TestScale1);
}

TEST_F(Transform_unit_tests, TranslateLocalSpace_CalledOnParent_OnlyWorldPositionModifiedInChild)
{
    auto parent = registry->Add<Entity>(EntityInfo{.position = TestPos1, .scale = TestScale1});
    auto child = registry->Add<Entity>(EntityInfo{.parent = parent});
    auto tParent = registry->Get<Transform>(parent);
    auto tChild = registry->Get<Transform>(child);
    tParent->TranslateLocalSpace(TestPos1);
    auto expectedPos = TestPos1 + TestPos1;
    EXPECT_EQ(tChild->GetPosition(), expectedPos);
    EXPECT_EQ(tChild->GetRotation(), Quaternion::Identity());
    EXPECT_EQ(tChild->GetScale(), TestScale1);
    EXPECT_EQ(tChild->GetLocalPosition(), Vector3::Zero());
    EXPECT_EQ(tChild->GetLocalRotation(), Quaternion::Identity());
    EXPECT_EQ(tChild->GetLocalScale(), Vector3::One());
}

TEST_F(Transform_unit_tests, Rotate_CalledOnRoot_OnlyRotationModified)
{
    auto e = registry->Add<Entity>(EntityInfo{.position = TestPos1, .scale = TestScale1});
    auto t = registry->Get<Transform>(e);
    t->Rotate(TestRotQuat2);
    EXPECT_EQ(t->GetPosition(), TestPos1);
    EXPECT_EQ(t->GetRotation(), TestRotQuat2);
    EXPECT_EQ(t->GetScale(), TestScale1);
    EXPECT_EQ(t->GetLocalPosition(), TestPos1);
    EXPECT_EQ(t->GetLocalRotation(), TestRotQuat2);
    EXPECT_EQ(t->GetLocalScale(), TestScale1);
}

TEST_F(Transform_unit_tests, Rotate_CalledOnParent_OnlyWorldRotationModifiedInChild)
{
    auto parent = registry->Add<Entity>(EntityInfo{.position = TestPos1, .scale = TestScale1});
    auto child = registry->Add<Entity>(EntityInfo{.parent = parent});
    auto tParent = registry->Get<Transform>(parent);
    auto tChild = registry->Get<Transform>(child);
    tParent->Rotate(TestRotQuat2);
    EXPECT_EQ(tChild->GetPosition(), TestPos1);
    EXPECT_EQ(tChild->GetRotation(), TestRotQuat2);
    EXPECT_EQ(tChild->GetScale(), TestScale1);
    EXPECT_EQ(tChild->GetLocalPosition(), Vector3::Zero());
    EXPECT_EQ(tChild->GetLocalRotation(), Quaternion::Identity());
    EXPECT_EQ(tChild->GetLocalScale(), Vector3::One());
}

TEST_F(Transform_unit_tests, RotateAxisAngleOverload_CalledOnRoot_OnlyRotationModified)
{
    auto e = registry->Add<Entity>(EntityInfo{.position = TestPos1, .scale = TestScale1});
    auto t = registry->Get<Transform>(e);
    auto expectedAxis = Vector3::Up();
    auto expectedAngle = 2.2f; 
    t->Rotate(expectedAxis, expectedAngle);
    auto actualQuat = t->GetRotation();
    auto expectedQuat = Quaternion::FromAxisAngle(expectedAxis, expectedAngle);
    EXPECT_EQ(t->GetPosition(), TestPos1);
    EXPECT_EQ(actualQuat, expectedQuat);
    EXPECT_EQ(t->GetScale(), TestScale1);
    EXPECT_EQ(t->GetLocalPosition(), TestPos1);
    EXPECT_EQ(t->GetLocalRotation(), expectedQuat);
    EXPECT_EQ(t->GetLocalScale(), TestScale1);
}

TEST_F(Transform_unit_tests, RotateAxisAngleOverload_CalledOnParent_OnlyWorldRotationModifiedInChild)
{
    auto parent = registry->Add<Entity>(EntityInfo{.position = TestPos1, .scale = TestScale1});
    auto child = registry->Add<Entity>(EntityInfo{.parent = parent});
    auto tParent = registry->Get<Transform>(parent);
    auto tChild = registry->Get<Transform>(child);
    auto expectedAxis = Vector3::Up();
    auto expectedAngle = 2.2f; 
    tParent->Rotate(expectedAxis, expectedAngle);
    EXPECT_EQ(tChild->GetPosition(), TestPos1);
    auto expectedQuat = Quaternion::FromAxisAngle(expectedAxis, expectedAngle);
    auto actualQuat = tChild->GetRotation();
    EXPECT_EQ(actualQuat, expectedQuat);
    EXPECT_EQ(tChild->GetScale(), TestScale1);
    EXPECT_EQ(tChild->GetLocalPosition(), Vector3::Zero());
    EXPECT_EQ(tChild->GetLocalRotation(), Quaternion::Identity());
    EXPECT_EQ(tChild->GetLocalScale(), Vector3::One());
}

TEST_F(Transform_unit_tests, GetChildren_ChildConstructor_AddsChildToParent)
{
    auto parent = registry->Add<Entity>(EntityInfo{});
    auto child = registry->Add<Entity>(EntityInfo{.parent = parent});
    auto tParent = registry->Get<Transform>(parent);
    auto children = tParent->GetChildren();
    auto actualCount = children.size();
    ASSERT_EQ(actualCount, 1u);
    EXPECT_EQ(children[0], child);
}

TEST_F(Transform_unit_tests, GetRoot_CalledOnRoot_ReturnsSelf)
{
    auto e = registry->Add<Entity>(EntityInfo{});
    auto t = registry->Get<Transform>(e);
    auto actual = t->GetRoot();
    EXPECT_EQ(actual, e);
}

TEST_F(Transform_unit_tests, GetRoot_WithOneDepthLevel_ReturnsParent)
{
    auto parent = registry->Add<Entity>(EntityInfo{});
    auto child = registry->Add<Entity>(EntityInfo{.parent = parent});
    auto tChild = registry->Get<Transform>(child);
    auto actual = tChild->GetRoot();
    EXPECT_EQ(actual, parent);
}

TEST_F(Transform_unit_tests, GetRoot_WithTwoDepthLevels_ReturnsParentsParent)
{
    auto root = registry->Add<Entity>(EntityInfo{});
    auto parent = registry->Add<Entity>(EntityInfo{.parent = root});
    auto child = registry->Add<Entity>(EntityInfo{.parent = parent});
    auto tChild = registry->Get<Transform>(child);
    auto actual = tChild->GetRoot();
    EXPECT_EQ(actual, root);
}

TEST_F(Transform_unit_tests, SetParent_RootChangedToChild_ParentAndChildUpdated)
{
    auto parent = registry->Add<Entity>(EntityInfo{});
    auto child = registry->Add<Entity>(EntityInfo{});
    auto tParent = registry->Get<Transform>(parent);
    auto tChild = registry->Get<Transform>(child);
    tChild->SetParent(parent);
    EXPECT_EQ(tChild->GetParent(), parent);
    auto children = tParent->GetChildren();
    ASSERT_EQ(children.size(), 1u);
    EXPECT_EQ(children[0], child);
}

TEST_F(Transform_unit_tests, SetParent_ChildReParented_ParentAndChildUpdated)
{
    auto oldParent = registry->Add<Entity>(EntityInfo{});
    auto newParent = registry->Add<Entity>(EntityInfo{});
    auto child = registry->Add<Entity>(EntityInfo{.parent = oldParent});
    auto tOldParent = registry->Get<Transform>(oldParent);
    auto tNewParent = registry->Get<Transform>(newParent);
    auto tChild = registry->Get<Transform>(child);
    tChild->SetParent(newParent);
    EXPECT_EQ(tChild->GetParent(), newParent);
    auto oldParentChildren = tOldParent->GetChildren();
    EXPECT_EQ(oldParentChildren.size(), 0u);
    auto newParentChildren = tNewParent->GetChildren();
    ASSERT_EQ(newParentChildren.size(), 1u);
    EXPECT_EQ(newParentChildren[0], child);
}

TEST_F(Transform_unit_tests, SetParent_ChildDetached_ParentAndChildUpdated)
{
    auto parent = registry->Add<Entity>(EntityInfo{});
    auto child = registry->Add<Entity>(EntityInfo{.parent = parent});
    auto tParent = registry->Get<Transform>(parent);
    auto tChild = registry->Get<Transform>(child);
    tChild->SetParent(Entity::Null());
    EXPECT_EQ(tChild->GetParent(), Entity::Null());
    auto children = tParent->GetChildren();
    EXPECT_EQ(children.size(), 0u);
}

int main(int argc, char ** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}