#include "gtest/gtest.h"
#include "ecs/Registry.h"

using namespace nc;

namespace nc
{
ecs::ComponentRegistry g_impl{10u};
Registry g_registry{g_impl};

namespace ecs::detail
{
void FreeComponentGroup::CommitStagedComponents() {}
}
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
        Registry* registry;

        Transform_unit_tests()
            : registry{&g_registry}
        {
        }

        ~Transform_unit_tests()
        {
            registry->CommitStagedChanges();
            registry->Clear();
        }

        static void SetUpTestSuite()
        {
            g_impl.RegisterType<nc::Tag>(10);
            g_impl.RegisterType<nc::Transform>(10);
            g_impl.RegisterType<nc::ecs::detail::FreeComponentGroup>(10);
            g_impl.RegisterType<nc::Hierarchy>(10);
        }
};

TEST_F(Transform_unit_tests, LocalPosition_CalledFromRoot_DecomposesPositionFromMatrix)
{
    auto e = registry->Add<Entity>(EntityInfo{.position = TestPos1});
    auto* t = registry->Get<Transform>(e);
    auto actual = t->LocalPosition();
    EXPECT_EQ(actual, TestPos1);
}

TEST_F(Transform_unit_tests, LocalPosition_CalledFromChild_ResultIsRelativeToParent)
{
    auto parent = registry->Add<Entity>(EntityInfo{.position = TestPos1});
    auto child = registry->Add<Entity>(EntityInfo{.position = Vector3::Zero(), .parent = parent});
    auto actual = registry->Get<Transform>(child)->LocalPosition();
    EXPECT_EQ(actual, Vector3::Zero());
}

TEST_F(Transform_unit_tests, Position_CalledFromRoot_DecomposesPositionFromMatrix)
{
    auto e = registry->Add<Entity>({.position = TestPos1});
    auto t = registry->Get<Transform>(e);
    auto actual = t->Position();
    EXPECT_EQ(actual, TestPos1);
}

TEST_F(Transform_unit_tests, Position_CalledFromChild_ResultIncludesParentPosition)
{
    auto parent = registry->Add<Entity>(EntityInfo{.position = TestPos1});
    auto child = registry->Add<Entity>(EntityInfo{.position = TestPos2, .parent = parent});
    auto t = registry->Get<Transform>(child);
    auto actual = t->Position();
    auto expected = TestPos1 + TestPos2;
    EXPECT_EQ(actual, expected);
}

TEST_F(Transform_unit_tests, LocalRotation_CalledFromRoot_DecomposesRotationFromMatrix)
{
    auto e = registry->Add<Entity>(EntityInfo{.rotation = TestRotQuat1});
    auto t = registry->Get<Transform>(e);
    auto actual = t->LocalRotation();
    EXPECT_EQ(actual, TestRotQuat1);
}

TEST_F(Transform_unit_tests, LocalRotation_CalledFromChild_ResultIsRelativeToParent)
{
    auto parent = registry->Add<Entity>(EntityInfo{.rotation = TestRotQuat2});
    auto child = registry->Add<Entity>(EntityInfo{.rotation = TestRotQuat2, .parent = parent});
    auto t = registry->Get<Transform>(child);
    auto actual = t->LocalRotation();
    EXPECT_EQ(actual, TestRotQuat2);
}

TEST_F(Transform_unit_tests, Rotation_CalledFromRoot_DecomposesRotationFromMatrix)
{
    auto e = registry->Add<Entity>(EntityInfo{.rotation = TestRotQuat2});
    auto t = registry->Get<Transform>(e);
    auto actual = t->Rotation();
    EXPECT_EQ(actual, TestRotQuat2);
}

TEST_F(Transform_unit_tests, Rotation_CalledFromChild_ResultIncludesParentRotation)
{
    auto parent = registry->Add<Entity>(EntityInfo{.rotation = TestRotQuat2});
    auto child = registry->Add<Entity>(EntityInfo{.rotation = TestRotQuat3, .parent = parent});
    auto t = registry->Get<Transform>(child);
    auto actual = t->Rotation();
    auto expected = Multiply(TestRotQuat2, TestRotQuat3);
    EXPECT_EQ(actual, expected);
}

TEST_F(Transform_unit_tests, LocalScale_CalledFromRoot_DecomposesScaleFromMatrix)
{
    auto e = registry->Add<Entity>(EntityInfo{.scale = TestScale1});
    auto t = registry->Get<Transform>(e);
    auto actual = t->LocalScale();
    EXPECT_EQ(actual, TestScale1);
}

TEST_F(Transform_unit_tests, LocalScale_CalledFromChild_ResultIsRelativeToParent)
{
    auto parent = registry->Add<Entity>(EntityInfo{.scale = TestScale2});
    auto child = registry->Add<Entity>(EntityInfo{.scale = TestScale3, .parent = parent});
    auto t = registry->Get<Transform>(child);
    auto actual = t->LocalScale();
    EXPECT_EQ(actual, TestScale3);
}

TEST_F(Transform_unit_tests, Scale_CalledFromRoot_DecomposesScaleFromMatrix)
{
    auto e = registry->Add<Entity>(EntityInfo{.scale = TestScale2});
    auto t = registry->Get<Transform>(e);
    auto actual = t->Scale();
    EXPECT_EQ(actual, TestScale2);
}

TEST_F(Transform_unit_tests, Scale_CalledFromChild_ResultIncludesParentScale)
{
    auto parent = registry->Add<Entity>(EntityInfo{.scale = TestScale2});
    auto child = registry->Add<Entity>(EntityInfo{.scale = TestScale3, .parent = parent});
    auto t = registry->Get<Transform>(child);
    auto actual = t->Scale();
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
    auto rot = Quaternion::FromEulerAngles(DegreesToRadians(90.0f), 0.0f, 0.0f);
    auto e = registry->Add<Entity>(EntityInfo{.rotation = rot});
    auto t = registry->Get<Transform>(e);
    auto actual = t->Up();
    auto expected = Vector3::Front();
    EXPECT_EQ(actual, expected);
}

TEST_F(Transform_unit_tests, Up_RotatedParent_ChildReturnsCorrectAxis)
{
    auto rot = Quaternion::FromEulerAngles(DegreesToRadians(90.0f), 0.0f, 0.0f);
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
    auto rot = Quaternion::FromEulerAngles(DegreesToRadians(90.0f), 0.0f, 0.0f);
    auto e = registry->Add<Entity>(EntityInfo{.rotation = rot});
    auto t = registry->Get<Transform>(e);
    auto actual = t->Forward();
    auto expected = Vector3::Down();
    EXPECT_EQ(actual, expected);
}

TEST_F(Transform_unit_tests, Forward_RotatedParent_ChildReturnsCorrectAxis)
{
    auto rot = Quaternion::FromEulerAngles(DegreesToRadians(90.0f), 0.0f, 0.0f);
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
    auto rot = Quaternion::FromEulerAngles(DegreesToRadians(90.0f), 0.0f, 0.0f);
    auto e = registry->Add<Entity>(EntityInfo{.rotation = rot});
    auto t = registry->Get<Transform>(e);
    auto actual = t->Right();
    auto expected = Vector3::Right();
    EXPECT_EQ(actual, expected);
}

TEST_F(Transform_unit_tests, Right_RotatedParent_ChildReturnsCorrectAxis)
{
    auto rot = Quaternion::FromEulerAngles(DegreesToRadians(90.0f), 0.0f, 0.0f);
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
    EXPECT_EQ(t->Position(), TestPos2);
    EXPECT_EQ(t->Rotation(), TestRotQuat2);
    EXPECT_EQ(t->Scale(), TestScale2);
    EXPECT_EQ(t->LocalPosition(), TestPos2);
    EXPECT_EQ(t->LocalRotation(), TestRotQuat2);
    EXPECT_EQ(t->LocalScale(), TestScale2);
}

TEST_F(Transform_unit_tests, Set_CalledOnParent_OnlyRecomposesChildWorldMatrix)
{
    auto parent = registry->Add<Entity>(EntityInfo{.position = TestPos1, .rotation = TestRotQuat1, .scale = TestScale1});
    auto child = registry->Add<Entity>(EntityInfo{.parent = parent});
    auto tParent = registry->Get<Transform>(parent);
    tParent->Set(TestPos2, TestRotQuat2, TestScale2);
    auto tChild = registry->Get<Transform>(child);
    EXPECT_EQ(tChild->Position(), TestPos2);
    EXPECT_EQ(tChild->Rotation(), TestRotQuat2);
    EXPECT_EQ(tChild->Scale(), TestScale2);
    EXPECT_EQ(tChild->LocalPosition(), Vector3::Zero());
    EXPECT_EQ(tChild->LocalRotation(), Quaternion::Identity());
    EXPECT_EQ(tChild->LocalScale(), Vector3::One());
}

TEST_F(Transform_unit_tests, SetAngleOverload_CalledOnRoot_CorrectlyRecomposesBothMatrices)
{
    auto e = registry->Add<Entity>(EntityInfo{.position = TestPos1, .rotation = TestRotQuat1, .scale = TestScale1});
    auto t = registry->Get<Transform>(e);
    t->Set(TestPos2, TestRotAngles, TestScale2);
    EXPECT_EQ(t->Position(), TestPos2);
    auto expectedRot = Quaternion::FromEulerAngles(TestRotAngles);
    EXPECT_EQ(t->Rotation(), expectedRot);
    EXPECT_EQ(t->Scale(), TestScale2);
    EXPECT_EQ(t->LocalPosition(), TestPos2);
    EXPECT_EQ(t->LocalRotation(), expectedRot);
    EXPECT_EQ(t->LocalScale(), TestScale2);
}

TEST_F(Transform_unit_tests, SetAngleOverload_CalledOnParent_OnlyRecomposesChildWorldMatrix)
{
    auto parent = registry->Add<Entity>(EntityInfo{.position = TestPos1, .rotation = TestRotQuat1, .scale = TestScale1});
    auto child = registry->Add<Entity>(EntityInfo{.parent = parent});
    auto* tParent = registry->Get<Transform>(parent);
    auto* tChild = registry->Get<Transform>(child);
    tParent->Set(TestPos2, TestRotAngles, TestScale2);
    EXPECT_EQ(tChild->Position(), TestPos2);
    auto expectedRot = Quaternion::FromEulerAngles(TestRotAngles);
    EXPECT_EQ(tChild->Rotation(), expectedRot);
    EXPECT_EQ(tChild->Scale(), TestScale2);
    EXPECT_EQ(tChild->LocalPosition(), Vector3::Zero());
    EXPECT_EQ(tChild->LocalRotation(), Quaternion::Identity());
    EXPECT_EQ(tChild->LocalScale(), Vector3::One());
}

TEST_F(Transform_unit_tests, SetPosition_CalledOnRoot_OnlyPositionModified)
{
    auto e = registry->Add<Entity>(EntityInfo{.position = TestPos1, .rotation = TestRotQuat1, .scale = TestScale1});
    auto t = registry->Get<Transform>(e);
    t->SetPosition(TestPos2);
    EXPECT_EQ(t->Position(), TestPos2);
    EXPECT_EQ(t->Rotation(), TestRotQuat1);
    EXPECT_EQ(t->Scale(), TestScale1);
    EXPECT_EQ(t->LocalPosition(), TestPos2);
    EXPECT_EQ(t->LocalRotation(), TestRotQuat1);
    EXPECT_EQ(t->LocalScale(), TestScale1);
}

TEST_F(Transform_unit_tests, SetPosition_CalledOnParent_OnlyWorldPositionModifiedInChild)
{
    auto parent = registry->Add<Entity>(EntityInfo{.position = TestPos1, .rotation = TestRotQuat1, .scale = TestScale1});
    auto child = registry->Add<Entity>(EntityInfo{.parent = parent});
    auto tParent = registry->Get<Transform>(parent);
    auto tChild = registry->Get<Transform>(child);
    tParent->SetPosition(TestPos2);
    EXPECT_EQ(tChild->Position(), TestPos2);
    EXPECT_EQ(tChild->Rotation(), TestRotQuat1);
    EXPECT_EQ(tChild->Scale(), TestScale1);
    EXPECT_EQ(tChild->LocalPosition(), Vector3::Zero());
    EXPECT_EQ(tChild->LocalRotation(), Quaternion::Identity());
    EXPECT_EQ(tChild->LocalScale(), Vector3::One());
}

TEST_F(Transform_unit_tests, SetRotation_CalledOnRoot_OnlyRotationModified)
{
    auto e = registry->Add<Entity>(EntityInfo{.position = TestPos1, .rotation = TestRotQuat1, .scale = TestScale1});
    auto t = registry->Get<Transform>(e);
    t->SetRotation(TestRotQuat2);
    EXPECT_EQ(t->Position(), TestPos1);
    EXPECT_EQ(t->Rotation(), TestRotQuat2);
    EXPECT_EQ(t->Scale(), TestScale1);
    EXPECT_EQ(t->LocalPosition(), TestPos1);
    EXPECT_EQ(t->LocalRotation(), TestRotQuat2);
    EXPECT_EQ(t->LocalScale(), TestScale1);
}

TEST_F(Transform_unit_tests, SetRotation_CalledOnParent_OnlyWorldRotationModifiedInChild)
{
    auto parent = registry->Add<Entity>(EntityInfo{.position = TestPos1, .rotation = TestRotQuat1, .scale = TestScale1});
    auto child = registry->Add<Entity>(EntityInfo{.parent = parent});
    auto tParent = registry->Get<Transform>(parent);
    auto tChild = registry->Get<Transform>(child);
    tParent->SetRotation(TestRotQuat2);
    EXPECT_EQ(tChild->Position(), TestPos1);
    EXPECT_EQ(tChild->Rotation(), TestRotQuat2);
    EXPECT_EQ(tChild->Scale(), TestScale1);
    EXPECT_EQ(tChild->LocalPosition(), Vector3::Zero());
    EXPECT_EQ(tChild->LocalRotation(), Quaternion::Identity());
    EXPECT_EQ(tChild->LocalScale(), Vector3::One());
}

TEST_F(Transform_unit_tests, SetRotationAnglesOverload_CalledOnRoot_OnlyRotationModified)
{
    auto e = registry->Add<Entity>(EntityInfo{.position = TestPos1, .rotation = TestRotQuat1, .scale = TestScale1});
    auto t = registry->Get<Transform>(e);
    t->SetRotation(TestRotAngles);
    EXPECT_EQ(t->Position(), TestPos1);
    auto expectedRot = Quaternion::FromEulerAngles(TestRotAngles);
    EXPECT_EQ(t->Rotation(), expectedRot);
    EXPECT_EQ(t->Scale(), TestScale1);
    EXPECT_EQ(t->LocalPosition(), TestPos1);
    EXPECT_EQ(t->LocalRotation(), expectedRot);
    EXPECT_EQ(t->LocalScale(), TestScale1);
}

TEST_F(Transform_unit_tests, SetRotationAnglesOverload_CalledOnParent_OnlyWorldRotationModifiedInChild)
{
    auto parent = registry->Add<Entity>(EntityInfo{.position = TestPos1, .rotation = TestRotQuat1, .scale = TestScale1});
    auto child = registry->Add<Entity>(EntityInfo{.parent = parent});
    auto tParent = registry->Get<Transform>(parent);
    auto tChild = registry->Get<Transform>(child);
    tParent->SetRotation(TestRotAngles);
    EXPECT_EQ(tChild->Position(), TestPos1);
    auto expectedRot = Quaternion::FromEulerAngles(TestRotAngles);
    EXPECT_EQ(tChild->Rotation(), expectedRot);
    EXPECT_EQ(tChild->Scale(), TestScale1);
    EXPECT_EQ(tChild->LocalPosition(), Vector3::Zero());
    EXPECT_EQ(tChild->LocalRotation(), Quaternion::Identity());
    EXPECT_EQ(tChild->LocalScale(), Vector3::One());
}

TEST_F(Transform_unit_tests, SetScale_CalledOnRoot_OnlyScaleModified)
{
    auto e = registry->Add<Entity>(EntityInfo{.position = TestPos1, .rotation = TestRotQuat1, .scale = TestScale1});
    auto t = registry->Get<Transform>(e);
    t->SetScale(TestScale2);
    EXPECT_EQ(t->Position(), TestPos1);
    EXPECT_EQ(t->Rotation(), TestRotQuat1);
    EXPECT_EQ(t->Scale(), TestScale2);
    EXPECT_EQ(t->LocalPosition(), TestPos1);
    EXPECT_EQ(t->LocalRotation(), TestRotQuat1);
    EXPECT_EQ(t->LocalScale(), TestScale2);
}

TEST_F(Transform_unit_tests, SetScale_CalledOnParent_OnlyWorldScaleModifiedInChild)
{
    auto parent = registry->Add<Entity>(EntityInfo{.position = TestPos1, .rotation = TestRotQuat1, .scale = TestScale1});
    auto child = registry->Add<Entity>(EntityInfo{.parent = parent});
    auto tParent = registry->Get<Transform>(parent);
    auto tChild = registry->Get<Transform>(child);
    tParent->SetScale(TestScale2);
    EXPECT_EQ(tChild->Position(), TestPos1);
    EXPECT_EQ(tChild->Rotation(), TestRotQuat1);
    EXPECT_EQ(tChild->Scale(), TestScale2);
    EXPECT_EQ(tChild->LocalPosition(), Vector3::Zero());
    EXPECT_EQ(tChild->LocalRotation(), Quaternion::Identity());
    EXPECT_EQ(tChild->LocalScale(), Vector3::One());
}

TEST_F(Transform_unit_tests, Translate_CalledOnRoot_OnlyPositionModified)
{
    auto e = registry->Add<Entity>(EntityInfo{.position = TestPos1, .rotation = TestRotQuat1, .scale = TestScale1});
    auto t = registry->Get<Transform>(e);
    t->Translate(TestPos1);
    auto expectedPos = TestPos1 + TestPos1;
    EXPECT_EQ(t->Position(), expectedPos);
    EXPECT_EQ(t->Rotation(), TestRotQuat1);
    EXPECT_EQ(t->Scale(), TestScale1);
    EXPECT_EQ(t->LocalPosition(), expectedPos);
    EXPECT_EQ(t->LocalRotation(), TestRotQuat1);
    EXPECT_EQ(t->LocalScale(), TestScale1);
}

TEST_F(Transform_unit_tests, Translate_CalledOnParent_OnlyWorldPositionModifiedInChild)
{
    auto parent = registry->Add<Entity>(EntityInfo{.position = TestPos1, .rotation = TestRotQuat1, .scale = TestScale1});
    auto child = registry->Add<Entity>(EntityInfo{.parent = parent});
    auto tParent = registry->Get<Transform>(parent);
    auto tChild = registry->Get<Transform>(child);
    tParent->Translate(TestPos1);
    auto expectedPos = TestPos1 + TestPos1;
    EXPECT_EQ(tChild->Position(), expectedPos);
    EXPECT_EQ(tChild->Rotation(), TestRotQuat1);
    EXPECT_EQ(tChild->Scale(), TestScale1);
    EXPECT_EQ(tChild->LocalPosition(), Vector3::Zero());
    EXPECT_EQ(tChild->LocalRotation(), Quaternion::Identity());
    EXPECT_EQ(tChild->LocalScale(), Vector3::One());
}

TEST_F(Transform_unit_tests, TranslateLocalSpace_CalledOnRoot_OnlyPositionModified)
{
    auto e = registry->Add<Entity>(EntityInfo{.position = TestPos1, .scale = TestScale1});
    auto t = registry->Get<Transform>(e);
    t->TranslateLocalSpace(TestPos1);
    auto expectedPos = TestPos1 + TestPos1;
    EXPECT_EQ(t->Position(), expectedPos);
    EXPECT_EQ(t->Rotation(), Quaternion::Identity());
    EXPECT_EQ(t->Scale(), TestScale1);
    EXPECT_EQ(t->LocalPosition(), expectedPos);
    EXPECT_EQ(t->LocalRotation(), Quaternion::Identity());
    EXPECT_EQ(t->LocalScale(), TestScale1);
}

TEST_F(Transform_unit_tests, TranslateLocalSpace_CalledOnParent_OnlyWorldPositionModifiedInChild)
{
    auto parent = registry->Add<Entity>(EntityInfo{.position = TestPos1, .scale = TestScale1});
    auto child = registry->Add<Entity>(EntityInfo{.parent = parent});
    auto tParent = registry->Get<Transform>(parent);
    auto tChild = registry->Get<Transform>(child);
    tParent->TranslateLocalSpace(TestPos1);
    auto expectedPos = TestPos1 + TestPos1;
    EXPECT_EQ(tChild->Position(), expectedPos);
    EXPECT_EQ(tChild->Rotation(), Quaternion::Identity());
    EXPECT_EQ(tChild->Scale(), TestScale1);
    EXPECT_EQ(tChild->LocalPosition(), Vector3::Zero());
    EXPECT_EQ(tChild->LocalRotation(), Quaternion::Identity());
    EXPECT_EQ(tChild->LocalScale(), Vector3::One());
}

TEST_F(Transform_unit_tests, Rotate_CalledOnRoot_OnlyRotationModified)
{
    auto e = registry->Add<Entity>(EntityInfo{.position = TestPos1, .scale = TestScale1});
    auto t = registry->Get<Transform>(e);
    t->Rotate(TestRotQuat2);
    EXPECT_EQ(t->Position(), TestPos1);
    EXPECT_EQ(t->Rotation(), TestRotQuat2);
    EXPECT_EQ(t->Scale(), TestScale1);
    EXPECT_EQ(t->LocalPosition(), TestPos1);
    EXPECT_EQ(t->LocalRotation(), TestRotQuat2);
    EXPECT_EQ(t->LocalScale(), TestScale1);
}

TEST_F(Transform_unit_tests, Rotate_CalledOnParent_OnlyWorldRotationModifiedInChild)
{
    auto parent = registry->Add<Entity>(EntityInfo{.position = TestPos1, .scale = TestScale1});
    auto child = registry->Add<Entity>(EntityInfo{.parent = parent});
    auto tParent = registry->Get<Transform>(parent);
    auto tChild = registry->Get<Transform>(child);
    tParent->Rotate(TestRotQuat2);
    EXPECT_EQ(tChild->Position(), TestPos1);
    EXPECT_EQ(tChild->Rotation(), TestRotQuat2);
    EXPECT_EQ(tChild->Scale(), TestScale1);
    EXPECT_EQ(tChild->LocalPosition(), Vector3::Zero());
    EXPECT_EQ(tChild->LocalRotation(), Quaternion::Identity());
    EXPECT_EQ(tChild->LocalScale(), Vector3::One());
}

TEST_F(Transform_unit_tests, RotateAxisAngleOverload_CalledOnRoot_OnlyRotationModified)
{
    auto e = registry->Add<Entity>(EntityInfo{.position = TestPos1, .scale = TestScale1});
    auto t = registry->Get<Transform>(e);
    auto expectedAxis = Vector3::Up();
    auto expectedAngle = 2.2f; 
    t->Rotate(expectedAxis, expectedAngle);
    auto actualQuat = t->Rotation();
    auto expectedQuat = Quaternion::FromAxisAngle(expectedAxis, expectedAngle);
    EXPECT_EQ(t->Position(), TestPos1);
    EXPECT_EQ(actualQuat, expectedQuat);
    EXPECT_EQ(t->Scale(), TestScale1);
    EXPECT_EQ(t->LocalPosition(), TestPos1);
    EXPECT_EQ(t->LocalRotation(), expectedQuat);
    EXPECT_EQ(t->LocalScale(), TestScale1);
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
    EXPECT_EQ(tChild->Position(), TestPos1);
    auto expectedQuat = Quaternion::FromAxisAngle(expectedAxis, expectedAngle);
    auto actualQuat = tChild->Rotation();
    EXPECT_EQ(actualQuat, expectedQuat);
    EXPECT_EQ(tChild->Scale(), TestScale1);
    EXPECT_EQ(tChild->LocalPosition(), Vector3::Zero());
    EXPECT_EQ(tChild->LocalRotation(), Quaternion::Identity());
    EXPECT_EQ(tChild->LocalScale(), Vector3::One());
}

// TEST_F(Transform_unit_tests, GetChildren_ChildConstructor_AddsChildToParent)
// {
//     auto parent = registry->Add<Entity>(EntityInfo{});
//     auto child = registry->Add<Entity>(EntityInfo{.parent = parent});
//     auto tParent = registry->Get<Transform>(parent);
//     auto children = tParent->Children();
//     auto actualCount = children.size();
//     ASSERT_EQ(actualCount, 1u);
//     EXPECT_EQ(children[0], child);
// }

// TEST_F(Transform_unit_tests, GetRoot_CalledOnRoot_ReturnsSelf)
// {
//     auto e = registry->Add<Entity>(EntityInfo{});
//     auto t = registry->Get<Transform>(e);
//     auto actual = t->Root();
//     EXPECT_EQ(actual, e);
// }

// TEST_F(Transform_unit_tests, GetRoot_WithOneDepthLevel_ReturnsParent)
// {
//     auto parent = registry->Add<Entity>(EntityInfo{});
//     auto child = registry->Add<Entity>(EntityInfo{.parent = parent});
//     auto tChild = registry->Get<Transform>(child);
//     auto actual = tChild->Root();
//     EXPECT_EQ(actual, parent);
// }

// TEST_F(Transform_unit_tests, GetRoot_WithTwoDepthLevels_ReturnsParentsParent)
// {
//     auto root = registry->Add<Entity>(EntityInfo{});
//     auto parent = registry->Add<Entity>(EntityInfo{.parent = root});
//     auto child = registry->Add<Entity>(EntityInfo{.parent = parent});
//     auto tChild = registry->Get<Transform>(child);
//     auto actual = tChild->Root();
//     EXPECT_EQ(actual, root);
// }

// TEST_F(Transform_unit_tests, SetParent_RootChangedToChild_ParentAndChildUpdated)
// {
//     auto parent = registry->Add<Entity>(EntityInfo{});
//     auto child = registry->Add<Entity>(EntityInfo{});
//     auto tParent = registry->Get<Transform>(parent);
//     auto tChild = registry->Get<Transform>(child);
//     tChild->SetParent(parent);
//     EXPECT_EQ(tChild->Parent(), parent);
//     auto children = tParent->Children();
//     ASSERT_EQ(children.size(), 1u);
//     EXPECT_EQ(children[0], child);
// }

// TEST_F(Transform_unit_tests, SetParent_ChildReParented_ParentAndChildUpdated)
// {
//     auto oldParent = registry->Add<Entity>(EntityInfo{});
//     auto newParent = registry->Add<Entity>(EntityInfo{});
//     auto child = registry->Add<Entity>(EntityInfo{.parent = oldParent});
//     auto tOldParent = registry->Get<Transform>(oldParent);
//     auto tNewParent = registry->Get<Transform>(newParent);
//     auto tChild = registry->Get<Transform>(child);
//     tChild->SetParent(newParent);
//     EXPECT_EQ(tChild->Parent(), newParent);
//     auto oldParentChildren = tOldParent->Children();
//     EXPECT_EQ(oldParentChildren.size(), 0u);
//     auto newParentChildren = tNewParent->Children();
//     ASSERT_EQ(newParentChildren.size(), 1u);
//     EXPECT_EQ(newParentChildren[0], child);
// }

// TEST_F(Transform_unit_tests, SetParent_ChildDetached_ParentAndChildUpdated)
// {
//     auto parent = registry->Add<Entity>(EntityInfo{});
//     auto child = registry->Add<Entity>(EntityInfo{.parent = parent});
//     auto tParent = registry->Get<Transform>(parent);
//     auto tChild = registry->Get<Transform>(child);
//     tChild->SetParent(Entity::Null());
//     EXPECT_EQ(tChild->Parent(), Entity::Null());
//     auto children = tParent->Children();
//     EXPECT_EQ(children.size(), 0u);
// }

int main(int argc, char ** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
