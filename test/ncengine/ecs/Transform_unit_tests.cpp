#include "gtest/gtest.h"
#include "ncengine/Events.h"
#include "ncengine/ecs/Ecs.h"
#include "ecs/NcEcsImpl.h"

using namespace nc;

namespace nc
{
ecs::ComponentRegistry g_impl{10u};

namespace ecs
{
EcsModule::EcsModule(ComponentRegistry& registry, SystemEvents& events) noexcept
    : m_registry{&registry},
      m_rebuildStaticConnection{events.rebuildStatics.Connect([](){})}
{
}

void EcsModule::OnBuildTaskGraph(task::UpdateTasks&, task::RenderTasks&) {}
void EcsModule::RunFrameLogic()
{
    UpdateWorldSpaceMatrices();
}

// Mock for updating transform worldspace matrices
void EcsModule::UpdateWorldSpaceMatrices()
{
    auto world = Ecs{*m_registry};
    for (auto entity : world.GetAll<Entity>())
    {
        auto& hierarchy = world.Get<Hierarchy>(entity);
        auto& transform = world.Get<Transform>(entity);
        if (!hierarchy.parent.Valid())
            transform.UpdateWorldMatrix();
        else
        {
            auto& parentTransform = world.Get<Transform>(hierarchy.parent);
            transform.UpdateWorldMatrix(parentTransform.TransformationMatrix());
        }
    }
}
} // namespace ecs
} // namespace nc

constexpr auto testLayer = Entity::layer_type{0u};
constexpr auto testFlags = Entity::Flags::None;
constexpr auto testHandle = Entity{1u, testLayer, testFlags};
constexpr auto testHandle2 = Entity{2u, testLayer, testFlags};
constexpr auto testHandle3 = Entity{3u, testLayer, testFlags};
constexpr auto testPos1 = Vector3{0.0f, 1.0f, 2.0f};
constexpr auto testPos2 = Vector3{1.5f, 1.5f, 1.5f};
constexpr auto testScale1 = Vector3{1.0f, 3.0f, 1.0f};
constexpr auto testScale2 = Vector3{10.0f, 1.0f, 3.7f};
constexpr auto testScale3 = Vector3::Splat(2.0f);
constexpr auto testRotAngles = Vector3{0.15f, 1.0f, 0.177f};
const auto testRotQuat1 = Quaternion::FromEulerAngles(0.0f, 0.0f, 90.0f);
const auto testRotQuat2 = Quaternion::FromEulerAngles(1.5f, 1.5f, 1.5f);
const auto testRotQuat3 = Quaternion::FromEulerAngles(2.5, 0.0f, 1.0f);


class Transform_unit_tests : public ::testing::Test
{
    public:
        ecs::Ecs world;
        SystemEvents events;
        ecs::EcsModule ecsModule;

        Transform_unit_tests()
            : world{g_impl},
              ecsModule{g_impl, events}
        {
        }

        ~Transform_unit_tests()
        {
            g_impl.CommitPendingChanges();
            g_impl.ClearSceneData();
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
    auto e = world.Emplace<Entity>(EntityInfo{.position = testPos1});
    auto& t = world.Get<Transform>(e);
    auto actual = t.LocalPosition();
    EXPECT_EQ(actual, testPos1);
}

TEST_F(Transform_unit_tests, LocalPosition_CalledFromChild_ResultIsRelativeToParent)
{
    auto parent = world.Emplace<Entity>(EntityInfo{.position = testPos1});
    auto child = world.Emplace<Entity>(EntityInfo{.position = Vector3::Zero(), .parent = parent});
    auto actual = world.Get<Transform>(child).LocalPosition();
    EXPECT_EQ(actual, Vector3::Zero());
}

TEST_F(Transform_unit_tests, Position_CalledFromRoot_DecomposesPositionFromMatrix)
{
    auto e = world.Emplace<Entity>({.position = testPos1});
    auto& t = world.Get<Transform>(e);
    auto actual = t.Position();
    EXPECT_EQ(actual, testPos1);
}

TEST_F(Transform_unit_tests, Position_CalledFromChild_ResultIncludesParentPosition)
{
    auto parent = world.Emplace<Entity>(EntityInfo{.position = testPos1});
    auto child = world.Emplace<Entity>(EntityInfo{.position = testPos2, .parent = parent});
    auto& t = world.Get<Transform>(child);
    auto actual = t.Position();
    auto expected = testPos1 + testPos2;
    EXPECT_EQ(actual, expected);
}

TEST_F(Transform_unit_tests, LocalRotation_CalledFromRoot_DecomposesRotationFromMatrix)
{
    auto e = world.Emplace<Entity>(EntityInfo{.rotation = testRotQuat1});
    auto& t = world.Get<Transform>(e);
    auto actual = t.LocalRotation();
    EXPECT_EQ(actual, testRotQuat1);
}

TEST_F(Transform_unit_tests, LocalRotation_CalledFromChild_ResultIsRelativeToParent)
{
    auto parent = world.Emplace<Entity>(EntityInfo{.rotation = testRotQuat2});
    auto child = world.Emplace<Entity>(EntityInfo{.rotation = testRotQuat2, .parent = parent});
    auto& t = world.Get<Transform>(child);
    auto actual = t.LocalRotation();
    EXPECT_EQ(actual, testRotQuat2);
}

TEST_F(Transform_unit_tests, Rotation_CalledFromRoot_DecomposesRotationFromMatrix)
{
    auto e = world.Emplace<Entity>(EntityInfo{.rotation = testRotQuat2});
    auto& t = world.Get<Transform>(e);
    auto actual = t.Rotation();
    EXPECT_EQ(actual, testRotQuat2);
}

TEST_F(Transform_unit_tests, Rotation_CalledFromChild_ResultIncludesParentRotation)
{
    auto parent = world.Emplace<Entity>(EntityInfo{.rotation = testRotQuat2});
    auto child = world.Emplace<Entity>(EntityInfo{.rotation = testRotQuat3, .parent = parent});
    auto& t = world.Get<Transform>(child);
    auto actual = t.Rotation();
    auto expected = Multiply(testRotQuat3, testRotQuat2);
    EXPECT_EQ(actual, expected);
}

TEST_F(Transform_unit_tests, LocalScale_CalledFromRoot_DecomposesScaleFromMatrix)
{
    auto e = world.Emplace<Entity>(EntityInfo{.scale = testScale1});
    auto& t = world.Get<Transform>(e);
    auto actual = t.LocalScale();
    EXPECT_EQ(actual, testScale1);
}

TEST_F(Transform_unit_tests, LocalScale_CalledFromChild_ResultIsRelativeToParent)
{
    auto parent = world.Emplace<Entity>(EntityInfo{.scale = testScale2});
    auto child = world.Emplace<Entity>(EntityInfo{.scale = testScale3, .parent = parent});
    auto& t = world.Get<Transform>(child);
    auto actual = t.LocalScale();
    EXPECT_EQ(actual, testScale3);
}

TEST_F(Transform_unit_tests, Scale_CalledFromRoot_DecomposesScaleFromMatrix)
{
    auto e = world.Emplace<Entity>(EntityInfo{.scale = testScale2});
    auto& t = world.Get<Transform>(e);
    auto actual = t.Scale();
    EXPECT_EQ(actual, testScale2);
}

TEST_F(Transform_unit_tests, Scale_CalledFromChild_ResultIncludesParentScale)
{
    auto parent = world.Emplace<Entity>(EntityInfo{.scale = testScale2});
    auto child = world.Emplace<Entity>(EntityInfo{.scale = testScale3, .parent = parent});
    auto& t = world.Get<Transform>(child);
    auto actual = t.Scale();
    auto expected = HadamardProduct(testScale2, testScale3);
    EXPECT_EQ(actual, expected);
}

TEST_F(Transform_unit_tests, Up_ReturnsNormalizedVector)
{
    auto e = world.Emplace<Entity>(EntityInfo{.rotation = testRotQuat2, .scale = testScale2});
    auto& t = world.Get<Transform>(e);
    auto actual = t.Up();
    auto expected = Normalize(actual);
    EXPECT_EQ(actual, expected);
}

TEST_F(Transform_unit_tests, Up_RotatedTransform_ReturnsCorrectAxis)
{
    auto rot = Quaternion::FromEulerAngles(DegreesToRadians(90.0f), 0.0f, 0.0f);
    auto e = world.Emplace<Entity>(EntityInfo{.rotation = rot});
    auto& t = world.Get<Transform>(e);
    auto actual = t.Up();
    auto expected = Vector3::Front();
    EXPECT_EQ(actual, expected);
}

TEST_F(Transform_unit_tests, Up_RotatedParent_ChildReturnsCorrectAxis)
{
    auto rot = Quaternion::FromEulerAngles(DegreesToRadians(90.0f), 0.0f, 0.0f);
    auto parent = world.Emplace<Entity>(EntityInfo{.rotation = rot});
    auto child = world.Emplace<Entity>(EntityInfo{.rotation = Quaternion::Identity(), .parent = parent});
    auto& t = world.Get<Transform>(child);
    auto actual = t.Up();
    auto expected = Vector3::Front();
    EXPECT_EQ(actual, expected);
}

TEST_F(Transform_unit_tests, Forward_ReturnsNormalizedVector)
{
    auto e = world.Emplace<Entity>(EntityInfo{.rotation = testRotQuat2, .scale = testScale2});
    auto& t = world.Get<Transform>(e);
    auto actual = t.Forward();
    auto expected = Normalize(actual);
    EXPECT_EQ(actual, expected);
}

TEST_F(Transform_unit_tests, Forward_RotatedTransform_ReturnsCorrectAxis)
{
    auto rot = Quaternion::FromEulerAngles(DegreesToRadians(90.0f), 0.0f, 0.0f);
    auto e = world.Emplace<Entity>(EntityInfo{.rotation = rot});
    auto& t = world.Get<Transform>(e);
    auto actual = t.Forward();
    auto expected = Vector3::Down();
    EXPECT_EQ(actual, expected);
}

TEST_F(Transform_unit_tests, Forward_RotatedParent_ChildReturnsCorrectAxis)
{
    auto rot = Quaternion::FromEulerAngles(DegreesToRadians(90.0f), 0.0f, 0.0f);
    auto parent = world.Emplace<Entity>(EntityInfo{.rotation = rot});
    auto child = world.Emplace<Entity>(EntityInfo{.rotation = Quaternion::Identity(), .parent = parent});
    auto& t = world.Get<Transform>(child);
    auto actual = t.Forward();
    auto expected = Vector3::Down();
    EXPECT_EQ(actual, expected);
}

TEST_F(Transform_unit_tests, Right_ReturnsNormalizedVector)
{
    auto e = world.Emplace<Entity>(EntityInfo{.rotation = testRotQuat2, .scale = testScale2});
    auto& t = world.Get<Transform>(e);
    auto actual = t.Right();
    auto expected = Normalize(actual);
    EXPECT_EQ(actual, expected);
}

TEST_F(Transform_unit_tests, Right_RotatedTransform_ReturnsCorrectAxis)
{
    auto rot = Quaternion::FromEulerAngles(DegreesToRadians(90.0f), 0.0f, 0.0f);
    auto e = world.Emplace<Entity>(EntityInfo{.rotation = rot});
    auto& t = world.Get<Transform>(e);
    auto actual = t.Right();
    auto expected = Vector3::Right();
    EXPECT_EQ(actual, expected);
}

TEST_F(Transform_unit_tests, Right_RotatedParent_ChildReturnsCorrectAxis)
{
    auto rot = Quaternion::FromEulerAngles(DegreesToRadians(90.0f), 0.0f, 0.0f);
    auto parent = world.Emplace<Entity>(EntityInfo{.rotation = rot});
    auto child = world.Emplace<Entity>(EntityInfo{.rotation = Quaternion::Identity(), .parent = parent});
    auto& t = world.Get<Transform>(child);
    auto actual = t.Right();
    auto expected = Vector3::Right();
    EXPECT_EQ(actual, expected);
}

TEST_F(Transform_unit_tests, Set_CalledOnRoot_CorrectlyRecomposesBothMatrices)
{
    auto e = world.Emplace<Entity>(EntityInfo{.position = testPos1, .rotation = testRotQuat1, .scale = testScale1});
    auto& t = world.Get<Transform>(e);
    t.Set(testPos2, testRotQuat2, testScale2);
    ecsModule.RunFrameLogic();
    EXPECT_EQ(t.Position(), testPos2);
    EXPECT_EQ(t.Rotation(), testRotQuat2);
    EXPECT_EQ(t.Scale(), testScale2);
    EXPECT_EQ(t.LocalPosition(), testPos2);
    EXPECT_EQ(t.LocalRotation(), testRotQuat2);
    EXPECT_EQ(t.LocalScale(), testScale2);
}

TEST_F(Transform_unit_tests, Set_CalledOnParent_OnlyRecomposesChildWorldMatrix)
{
    auto parent = world.Emplace<Entity>(EntityInfo{.position = testPos1, .rotation = testRotQuat1, .scale = testScale1});
    auto child = world.Emplace<Entity>(EntityInfo{.parent = parent});
    auto& tParent = world.Get<Transform>(parent);
    tParent.Set(testPos2, testRotQuat2, testScale2);
    ecsModule.RunFrameLogic();
    auto& tChild = world.Get<Transform>(child);
    EXPECT_EQ(tChild.Position(), testPos2);
    EXPECT_EQ(tChild.Rotation(), testRotQuat2);
    EXPECT_EQ(tChild.Scale(), testScale2);
    EXPECT_EQ(tChild.LocalPosition(), Vector3::Zero());
    EXPECT_EQ(tChild.LocalRotation(), Quaternion::Identity());
    EXPECT_EQ(tChild.LocalScale(), Vector3::One());
}

TEST_F(Transform_unit_tests, SetAngleOverload_CalledOnRoot_CorrectlyRecomposesBothMatrices)
{
    auto e = world.Emplace<Entity>(EntityInfo{.position = testPos1, .rotation = testRotQuat1, .scale = testScale1});
    auto& t = world.Get<Transform>(e);
    t.Set(testPos2, testRotAngles, testScale2);
    ecsModule.RunFrameLogic();
    EXPECT_EQ(t.Position(), testPos2);
    auto expectedRot = Quaternion::FromEulerAngles(testRotAngles);
    EXPECT_EQ(t.Rotation(), expectedRot);
    EXPECT_EQ(t.Scale(), testScale2);
    EXPECT_EQ(t.LocalPosition(), testPos2);
    EXPECT_EQ(t.LocalRotation(), expectedRot);
    EXPECT_EQ(t.LocalScale(), testScale2);
}

TEST_F(Transform_unit_tests, SetAngleOverload_CalledOnParent_OnlyRecomposesChildWorldMatrix)
{
    auto parent = world.Emplace<Entity>(EntityInfo{.position = testPos1, .rotation = testRotQuat1, .scale = testScale1});
    auto child = world.Emplace<Entity>(EntityInfo{.parent = parent});
    auto& tParent = world.Get<Transform>(parent);
    auto& tChild = world.Get<Transform>(child);
    tParent.Set(testPos2, testRotAngles, testScale2);
    ecsModule.RunFrameLogic();
    EXPECT_EQ(tChild.Position(), testPos2);
    auto expectedRot = Quaternion::FromEulerAngles(testRotAngles);
    EXPECT_EQ(tChild.Rotation(), expectedRot);
    EXPECT_EQ(tChild.Scale(), testScale2);
    EXPECT_EQ(tChild.LocalPosition(), Vector3::Zero());
    EXPECT_EQ(tChild.LocalRotation(), Quaternion::Identity());
    EXPECT_EQ(tChild.LocalScale(), Vector3::One());
}

TEST_F(Transform_unit_tests, SetPosition_CalledOnRoot_OnlyPositionModified)
{
    auto e = world.Emplace<Entity>(EntityInfo{.position = testPos1, .rotation = testRotQuat1, .scale = testScale1});
    auto& t = world.Get<Transform>(e);
    t.SetPosition(testPos2);
    ecsModule.RunFrameLogic();
    EXPECT_EQ(t.Position(), testPos2);
    EXPECT_EQ(t.Rotation(), testRotQuat1);
    EXPECT_EQ(t.Scale(), testScale1);
    EXPECT_EQ(t.LocalPosition(), testPos2);
    EXPECT_EQ(t.LocalRotation(), testRotQuat1);
    EXPECT_EQ(t.LocalScale(), testScale1);
}

TEST_F(Transform_unit_tests, SetPosition_CalledOnParent_OnlyWorldPositionModifiedInChild)
{
    auto parent = world.Emplace<Entity>(EntityInfo{.position = testPos1, .rotation = testRotQuat1, .scale = testScale1});
    auto child = world.Emplace<Entity>(EntityInfo{.parent = parent});
    auto& tParent = world.Get<Transform>(parent);
    auto& tChild = world.Get<Transform>(child);
    tParent.SetPosition(testPos2);
    ecsModule.RunFrameLogic();
    EXPECT_EQ(tChild.Position(), testPos2);
    EXPECT_EQ(tChild.Rotation(), testRotQuat1);
    EXPECT_EQ(tChild.Scale(), testScale1);
    EXPECT_EQ(tChild.LocalPosition(), Vector3::Zero());
    EXPECT_EQ(tChild.LocalRotation(), Quaternion::Identity());
    EXPECT_EQ(tChild.LocalScale(), Vector3::One());
}

TEST_F(Transform_unit_tests, SetRotation_CalledOnRoot_OnlyRotationModified)
{
    auto e = world.Emplace<Entity>(EntityInfo{.position = testPos1, .rotation = testRotQuat1, .scale = testScale1});
    auto& t = world.Get<Transform>(e);
    t.SetRotation(testRotQuat2);
    ecsModule.RunFrameLogic();
    EXPECT_EQ(t.Position(), testPos1);
    EXPECT_EQ(t.Rotation(), testRotQuat2);
    EXPECT_EQ(t.Scale(), testScale1);
    EXPECT_EQ(t.LocalPosition(), testPos1);
    EXPECT_EQ(t.LocalRotation(), testRotQuat2);
    EXPECT_EQ(t.LocalScale(), testScale1);
}

TEST_F(Transform_unit_tests, SetRotation_CalledOnParent_OnlyWorldRotationModifiedInChild)
{
    auto parent = world.Emplace<Entity>(EntityInfo{.position = testPos1, .rotation = testRotQuat1, .scale = testScale1});
    auto child = world.Emplace<Entity>(EntityInfo{.parent = parent});
    auto& tParent = world.Get<Transform>(parent);
    auto& tChild = world.Get<Transform>(child);
    tParent.SetRotation(testRotQuat2);
    ecsModule.RunFrameLogic();
    EXPECT_EQ(tChild.Position(), testPos1);
    EXPECT_EQ(tChild.Rotation(), testRotQuat2);
    EXPECT_EQ(tChild.Scale(), testScale1);
    EXPECT_EQ(tChild.LocalPosition(), Vector3::Zero());
    EXPECT_EQ(tChild.LocalRotation(), Quaternion::Identity());
    EXPECT_EQ(tChild.LocalScale(), Vector3::One());
}

TEST_F(Transform_unit_tests, SetRotationAnglesOverload_CalledOnRoot_OnlyRotationModified)
{
    auto e = world.Emplace<Entity>(EntityInfo{.position = testPos1, .rotation = testRotQuat1, .scale = testScale1});
    auto& t = world.Get<Transform>(e);
    t.SetRotation(testRotAngles);
    ecsModule.RunFrameLogic();
    EXPECT_EQ(t.Position(), testPos1);
    auto expectedRot = Quaternion::FromEulerAngles(testRotAngles);
    EXPECT_EQ(t.Rotation(), expectedRot);
    EXPECT_EQ(t.Scale(), testScale1);
    EXPECT_EQ(t.LocalPosition(), testPos1);
    EXPECT_EQ(t.LocalRotation(), expectedRot);
    EXPECT_EQ(t.LocalScale(), testScale1);
}

TEST_F(Transform_unit_tests, SetRotationAnglesOverload_CalledOnParent_OnlyWorldRotationModifiedInChild)
{
    auto parent = world.Emplace<Entity>(EntityInfo{.position = testPos1, .rotation = testRotQuat1, .scale = testScale1});
    auto child = world.Emplace<Entity>(EntityInfo{.parent = parent});
    auto& tParent = world.Get<Transform>(parent);
    auto& tChild = world.Get<Transform>(child);
    tParent.SetRotation(testRotAngles);
    ecsModule.RunFrameLogic();
    EXPECT_EQ(tChild.Position(), testPos1);
    auto expectedRot = Quaternion::FromEulerAngles(testRotAngles);
    EXPECT_EQ(tChild.Rotation(), expectedRot);
    EXPECT_EQ(tChild.Scale(), testScale1);
    EXPECT_EQ(tChild.LocalPosition(), Vector3::Zero());
    EXPECT_EQ(tChild.LocalRotation(), Quaternion::Identity());
    EXPECT_EQ(tChild.LocalScale(), Vector3::One());
}

TEST_F(Transform_unit_tests, SetScale_CalledOnRoot_OnlyScaleModified)
{
    auto e = world.Emplace<Entity>(EntityInfo{.position = testPos1, .rotation = testRotQuat1, .scale = testScale1});
    auto& t = world.Get<Transform>(e);
    t.SetScale(testScale2);
    ecsModule.RunFrameLogic();
    EXPECT_EQ(t.Position(), testPos1);
    EXPECT_EQ(t.Rotation(), testRotQuat1);
    EXPECT_EQ(t.Scale(), testScale2);
    EXPECT_EQ(t.LocalPosition(), testPos1);
    EXPECT_EQ(t.LocalRotation(), testRotQuat1);
    EXPECT_EQ(t.LocalScale(), testScale2);
}

TEST_F(Transform_unit_tests, SetScale_CalledOnParent_OnlyWorldScaleModifiedInChild)
{
    auto parent = world.Emplace<Entity>(EntityInfo{.position = testPos1, .rotation = testRotQuat1, .scale = testScale1});
    auto child = world.Emplace<Entity>(EntityInfo{.parent = parent});
    auto& tParent = world.Get<Transform>(parent);
    auto& tChild = world.Get<Transform>(child);
    tParent.SetScale(testScale2);
    ecsModule.RunFrameLogic();
    EXPECT_EQ(tChild.Position(), testPos1);
    EXPECT_EQ(tChild.Rotation(), testRotQuat1);
    EXPECT_EQ(tChild.Scale(), testScale2);
    EXPECT_EQ(tChild.LocalPosition(), Vector3::Zero());
    EXPECT_EQ(tChild.LocalRotation(), Quaternion::Identity());
    EXPECT_EQ(tChild.LocalScale(), Vector3::One());
}

TEST_F(Transform_unit_tests, Translate_CalledOnRoot_OnlyPositionModified)
{
    auto e = world.Emplace<Entity>(EntityInfo{.position = testPos1, .rotation = testRotQuat1, .scale = testScale1});
    auto& t = world.Get<Transform>(e);
    t.Translate(testPos1);
    ecsModule.RunFrameLogic();
    auto expectedPos = testPos1 + testPos1;
    EXPECT_EQ(t.Position(), expectedPos);
    EXPECT_EQ(t.Rotation(), testRotQuat1);
    EXPECT_EQ(t.Scale(), testScale1);
    EXPECT_EQ(t.LocalPosition(), expectedPos);
    EXPECT_EQ(t.LocalRotation(), testRotQuat1);
    EXPECT_EQ(t.LocalScale(), testScale1);
}

TEST_F(Transform_unit_tests, Translate_CalledOnParent_OnlyWorldPositionModifiedInChild)
{
    auto parent = world.Emplace<Entity>(EntityInfo{.position = testPos1, .rotation = testRotQuat1, .scale = testScale1});
    auto child = world.Emplace<Entity>(EntityInfo{.parent = parent});
    auto& tParent = world.Get<Transform>(parent);
    auto& tChild = world.Get<Transform>(child);
    tParent.Translate(testPos1);
    ecsModule.RunFrameLogic();
    auto expectedPos = testPos1 + testPos1;
    EXPECT_EQ(tChild.Position(), expectedPos);
    EXPECT_EQ(tChild.Rotation(), testRotQuat1);
    EXPECT_EQ(tChild.Scale(), testScale1);
    EXPECT_EQ(tChild.LocalPosition(), Vector3::Zero());
    EXPECT_EQ(tChild.LocalRotation(), Quaternion::Identity());
    EXPECT_EQ(tChild.LocalScale(), Vector3::One());
}

TEST_F(Transform_unit_tests, TranslateLocalSpace_CalledOnRoot_OnlyPositionModified)
{
    auto e = world.Emplace<Entity>(EntityInfo{.position = testPos1, .scale = testScale1});
    auto& t = world.Get<Transform>(e);
    t.TranslateLocalSpace(testPos1);
    ecsModule.RunFrameLogic();
    auto expectedPos = testPos1 + testPos1;
    EXPECT_EQ(t.Position(), expectedPos);
    EXPECT_EQ(t.Rotation(), Quaternion::Identity());
    EXPECT_EQ(t.Scale(), testScale1);
    EXPECT_EQ(t.LocalPosition(), expectedPos);
    EXPECT_EQ(t.LocalRotation(), Quaternion::Identity());
    EXPECT_EQ(t.LocalScale(), testScale1);
}

TEST_F(Transform_unit_tests, TranslateLocalSpace_CalledOnParent_OnlyWorldPositionModifiedInChild)
{
    auto parent = world.Emplace<Entity>(EntityInfo{.position = testPos1, .scale = testScale1});
    auto child = world.Emplace<Entity>(EntityInfo{.parent = parent});
    auto& tParent = world.Get<Transform>(parent);
    auto& tChild = world.Get<Transform>(child);
    tParent.TranslateLocalSpace(testPos1);
    ecsModule.RunFrameLogic();
    auto expectedPos = testPos1 + testPos1;
    EXPECT_EQ(tChild.Position(), expectedPos);
    EXPECT_EQ(tChild.Rotation(), Quaternion::Identity());
    EXPECT_EQ(tChild.Scale(), testScale1);
    EXPECT_EQ(tChild.LocalPosition(), Vector3::Zero());
    EXPECT_EQ(tChild.LocalRotation(), Quaternion::Identity());
    EXPECT_EQ(tChild.LocalScale(), Vector3::One());
}

TEST_F(Transform_unit_tests, Rotate_CalledOnRoot_OnlyRotationModified)
{
    auto e = world.Emplace<Entity>(EntityInfo{.position = testPos1, .scale = testScale1});
    auto& t = world.Get<Transform>(e);
    t.Rotate(testRotQuat2);
    ecsModule.RunFrameLogic();
    EXPECT_EQ(t.Position(), testPos1);
    EXPECT_EQ(t.Rotation(), testRotQuat2);
    EXPECT_EQ(t.Scale(), testScale1);
    EXPECT_EQ(t.LocalPosition(), testPos1);
    EXPECT_EQ(t.LocalRotation(), testRotQuat2);
    EXPECT_EQ(t.LocalScale(), testScale1);
}

TEST_F(Transform_unit_tests, Rotate_CalledOnParent_OnlyWorldRotationModifiedInChild)
{
    auto parent = world.Emplace<Entity>(EntityInfo{.position = testPos1, .scale = testScale1});
    auto child = world.Emplace<Entity>(EntityInfo{.parent = parent});
    auto& tParent = world.Get<Transform>(parent);
    auto& tChild = world.Get<Transform>(child);
    tParent.Rotate(testRotQuat2);
    ecsModule.RunFrameLogic();
    EXPECT_EQ(tChild.Position(), testPos1);
    EXPECT_EQ(tChild.Rotation(), testRotQuat2);
    EXPECT_EQ(tChild.Scale(), testScale1);
    EXPECT_EQ(tChild.LocalPosition(), Vector3::Zero());
    EXPECT_EQ(tChild.LocalRotation(), Quaternion::Identity());
    EXPECT_EQ(tChild.LocalScale(), Vector3::One());
}

TEST_F(Transform_unit_tests, RotateAxisAngleOverload_CalledOnRoot_OnlyRotationModified)
{
    auto e = world.Emplace<Entity>(EntityInfo{.position = testPos1, .scale = testScale1});
    auto& t = world.Get<Transform>(e);
    auto expectedAxis = Vector3::Up();
    auto expectedAngle = 2.2f; 
    t.Rotate(expectedAxis, expectedAngle);
    ecsModule.RunFrameLogic();
    auto actualQuat = t.Rotation();
    auto expectedQuat = Quaternion::FromAxisAngle(expectedAxis, expectedAngle);
    EXPECT_EQ(t.Position(), testPos1);
    EXPECT_EQ(actualQuat, expectedQuat);
    EXPECT_EQ(t.Scale(), testScale1);
    EXPECT_EQ(t.LocalPosition(), testPos1);
    EXPECT_EQ(t.LocalRotation(), expectedQuat);
    EXPECT_EQ(t.LocalScale(), testScale1);
}

TEST_F(Transform_unit_tests, RotateAxisAngleOverload_CalledOnParent_OnlyWorldRotationModifiedInChild)
{
    auto parent = world.Emplace<Entity>(EntityInfo{.position = testPos1, .scale = testScale1});
    auto child = world.Emplace<Entity>(EntityInfo{.parent = parent});
    auto& tParent = world.Get<Transform>(parent);
    auto& tChild = world.Get<Transform>(child);
    auto expectedAxis = Vector3::Up();
    auto expectedAngle = 2.2f; 
    tParent.Rotate(expectedAxis, expectedAngle);
    ecsModule.RunFrameLogic();
    EXPECT_EQ(tChild.Position(), testPos1);
    auto expectedQuat = Quaternion::FromAxisAngle(expectedAxis, expectedAngle);
    auto actualQuat = tChild.Rotation();
    EXPECT_EQ(actualQuat, expectedQuat);
    EXPECT_EQ(tChild.Scale(), testScale1);
    EXPECT_EQ(tChild.LocalPosition(), Vector3::Zero());
    EXPECT_EQ(tChild.LocalRotation(), Quaternion::Identity());
    EXPECT_EQ(tChild.LocalScale(), Vector3::One());
}

int main(int argc, char ** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
