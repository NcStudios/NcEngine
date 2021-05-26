#include "gtest/gtest.h"
#include "component/Component.h"
#include "component/Transform.h"
#include "ecs/Registry.h"

using namespace nc;
using namespace nc::ecs;

namespace nc
{
    void Entity::SendOnDestroy() {}

    Entity::Entity(EntityHandle handle, std::string tag, physics::Layer layer) noexcept
        : Handle{handle}, Tag{tag}, Layer{layer}, m_userComponents{}
    {}

    Transform::Transform(EntityHandle handle, const Vector3&, const Quaternion&, const Vector3&, EntityHandle parent)
        : ComponentBase{handle}, m_localMatrix{}, m_worldMatrix{}, m_parent{parent}, m_children{}
    {}

    Quaternion::Quaternion(float X, float Y, float Z, float W)
        : x{X}, y{Y}, z{Z}, w{W}
    {}
}

struct Fake1 : public Component
{
    Fake1(EntityHandle handle, int v)
        : Component{handle},
          value{v}
    {}

    int value;
};

struct Fake2 : public Component
{
    Fake2(EntityHandle handle)
        : Component{handle}
    {}
};

constexpr auto Handle1 = EntityHandle{0u, 0u, 0u, 0u};
constexpr auto Handle2 = EntityHandle{1u, 0u, 0u, 0u};
constexpr auto Handle3 = EntityHandle{2u, 0u, 0u, 0u};
constexpr auto Handle4 = EntityHandle{3u, 0u, 0u, 0u};
constexpr auto Handle5 = EntityHandle{4u, 0u, 0u, 0u};
const auto TestInfo = EntityInfo{};

class Registry_unit_tests : public ::testing::Test
{
    public:
        size_t maxEntities = 5u;
        Registry<Transform, Fake1, Fake2> registry;

        Registry_unit_tests()
            : registry{maxEntities}
        {}

        ~Registry_unit_tests()
        {
            registry.Clear();
        }
};

TEST_F(Registry_unit_tests, CreateEntity_ReturnsNonNull)
{
    auto handle = registry.CreateEntity(TestInfo);
    EXPECT_TRUE(handle.Valid());
}

TEST_F(Registry_unit_tests, CreateEntity_ConsecutiveCalls_ReturnsDistinctHandles)
{
    auto handle1 = registry.CreateEntity(TestInfo);
    auto handle2 = registry.CreateEntity(TestInfo);
    EXPECT_NE(handle1, handle2);
}

TEST_F(Registry_unit_tests, CreateEntity_AddsTransform)
{
    auto handle = registry.CreateEntity(TestInfo);
    EXPECT_TRUE(registry.HasComponent<Transform>(handle));
}

TEST_F(Registry_unit_tests, DestroyEntity_EntityExists_EntityRemovedFromActiveList)
{
    auto handle = registry.CreateEntity(TestInfo);
    registry.CommitStagedChanges();
    auto entities = registry.GetActiveEntities();
    EXPECT_EQ(entities.size(), 1u);
    registry.DestroyEntity(handle);
    entities = registry.GetActiveEntities();
    EXPECT_EQ(entities.size(), 0u);
}

TEST_F(Registry_unit_tests, DestroyEntity_DoesNotExist_Throws)
{
    EXPECT_THROW(registry.DestroyEntity(Handle1), std::runtime_error);
}

TEST_F(Registry_unit_tests, EntityExists_Exists_ReturnsTrue)
{
    auto handle = registry.CreateEntity(TestInfo);
    EXPECT_TRUE(registry.EntityExists(handle));
}

TEST_F(Registry_unit_tests, EntityExists_DoesNotExist_ReturnsFalse)
{
    EXPECT_FALSE(registry.EntityExists(Handle1));
}

TEST_F(Registry_unit_tests, EntityExists_AfterRemove_ReturnsFalse)
{
    auto handle = registry.CreateEntity(TestInfo);
    registry.CommitStagedChanges();
    registry.DestroyEntity(handle);
    EXPECT_FALSE(registry.EntityExists(handle));
}

TEST_F(Registry_unit_tests, GetEntity_Exists_ReturnsPtr)
{
    auto handle = registry.CreateEntity(TestInfo);
    auto* ptr = registry.GetEntity(handle);
    EXPECT_NE(ptr, nullptr);
    EXPECT_EQ(handle, ptr->Handle);
}

TEST_F(Registry_unit_tests, GetEntity_DoesNotExist_ReturnsNull)
{
    EXPECT_EQ(registry.GetEntity(Handle1), nullptr);
}

TEST_F(Registry_unit_tests, GetEntity_AfterRemove_ReturnsNull)
{
    auto handle = registry.CreateEntity(TestInfo);
    registry.CommitStagedChanges();
    registry.DestroyEntity(handle);
    EXPECT_EQ(registry.GetEntity(handle), nullptr);
}

TEST_F(Registry_unit_tests, GetEntityTagOverload_ExistsReturnsPtr)
{
    auto handle = registry.CreateEntity({.tag = "Test"});
    auto* ptr = registry.GetEntity(std::string{"Test"});
    EXPECT_NE(ptr, nullptr);
    EXPECT_EQ(ptr->Handle, handle);
}

TEST_F(Registry_unit_tests, GetEntityTagOverload_DoesNotExist_ReturnsNull)
{
    EXPECT_EQ(registry.GetEntity(std::string{"Test"}), nullptr);
}

TEST_F(Registry_unit_tests, GetEntityTagOverload_AfterRemove_ReturnsNull)
{
    auto handle = registry.CreateEntity({.tag = "Test"});
    registry.CommitStagedChanges();
    registry.DestroyEntity(handle);
    EXPECT_EQ(registry.GetEntity(std::string{"Test"}), nullptr);
}

TEST_F(Registry_unit_tests, AddComponent_ValidCall_ConstructsObject)
{
    auto* ptr = registry.AddComponent<Fake1>(Handle1, 1);
    EXPECT_EQ(ptr->GetParentHandle(), Handle1);
    EXPECT_EQ(ptr->value, 1);
}

TEST_F(Registry_unit_tests, AddComponent_ReplaceAfterRemove_ConstructsObject)
{
    registry.AddComponent<Fake1>(Handle1, 1);
    registry.RemoveComponent<Fake1>(Handle1);
    auto* ptr = registry.AddComponent<Fake1>(Handle1, 2);
    EXPECT_EQ(ptr->GetParentHandle(), Handle1);
    EXPECT_EQ(ptr->value, 2);
}

TEST_F(Registry_unit_tests, AddComponent_DoubleCall_Throws)
{
    registry.AddComponent<Fake1>(Handle1, 1);
    EXPECT_THROW(registry.AddComponent<Fake1>(Handle1, 1), std::runtime_error);
}

TEST_F(Registry_unit_tests, RemoveComponent_ComponentExists_ReturnsTrue)
{
    registry.AddComponent<Fake1>(Handle1, 1);
    auto actual = registry.RemoveComponent<Fake1>(Handle1);
    EXPECT_TRUE(actual);
}

TEST_F(Registry_unit_tests, RemoveComponent_EmptyRegistry_ReturnsFalse)
{
    auto actual = registry.RemoveComponent<Fake1>(Handle1);
    EXPECT_FALSE(actual);
}

TEST_F(Registry_unit_tests, RemoveComponent_ComponentDoesNotExist_ReturnsFalse)
{
    registry.AddComponent<Fake1>(Handle1, 1u);
    auto actual = registry.RemoveComponent<Fake1>(Handle2);
    EXPECT_FALSE(actual);
}

TEST_F(Registry_unit_tests, RemoveComponent_DoubleCall_ReturnsFalse)
{
    registry.AddComponent<Fake1>(Handle1, 1u);
    registry.RemoveComponent<Fake1>(Handle1);
    auto actual = registry.RemoveComponent<Fake1>(Handle1);
    EXPECT_FALSE(actual);
}

TEST_F(Registry_unit_tests, RemoveComponent_AfterClear_ReturnsFalse)
{
    registry.AddComponent<Fake1>(Handle1, 1u);
    registry.Clear();
    auto actual = registry.RemoveComponent<Fake1>(Handle1);
    EXPECT_FALSE(actual);
}

TEST_F(Registry_unit_tests, RemoveComponent_IncorrectType_ReturnsFalse)
{
    registry.AddComponent<Fake1>(Handle1, 1);
    auto actual = registry.RemoveComponent<Fake2>(Handle1);
    EXPECT_FALSE(actual);
}

int main(int argc, char ** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}