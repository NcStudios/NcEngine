#include "gtest/gtest.h"
#include "component/AutoComponentGroup.h"
#include "component/Registry.h"
#include "component/Tag.h"
#include "component/Transform.h"

using namespace nc;
using namespace nc::ecs;

namespace nc
{
    AutoComponentGroup::AutoComponentGroup(Entity)
    {}

    void AutoComponentGroup::SendOnDestroy() {}

    Transform::Transform(Entity entity, const Vector3&, const Quaternion&, const Vector3&, Entity parent)
        : ComponentBase{entity}, m_localMatrix{}, m_worldMatrix{}, m_parent{parent}, m_children{}
    {}

    Quaternion::Quaternion(float X, float Y, float Z, float W)
        : x{X}, y{Y}, z{Z}, w{W}
    {}
}

struct Fake1 : public ComponentBase
{
    Fake1(Entity entity, int v)
        : ComponentBase{entity},
          value{v}
    {}

    int value;
};

struct Fake2 : public ComponentBase
{
    Fake2(Entity entity, int v)
        : ComponentBase{entity},
          value{v}
    {}

    int value;
};

struct FakeAutoComponent : public AutoComponent
{
    FakeAutoComponent(Entity entity, int v)
        : AutoComponent{entity}, value{v}
    {}

    int value;
};

constexpr auto Handle1 = Entity{0u, 0u, 0u};
constexpr auto Handle2 = Entity{1u, 0u, 0u};
constexpr auto Handle3 = Entity{2u, 0u, 0u};
constexpr auto Handle4 = Entity{3u, 0u, 0u};
constexpr auto Handle5 = Entity{4u, 0u, 0u};
const auto TestInfo = EntityInfo{};

class Registry_unit_tests : public ::testing::Test
{
    public:
        size_t maxEntities = 10u;
        using type_list = RegistryTypeList<Transform, AutoComponentGroup, Tag, Fake1, Fake2>;
        Registry<type_list> registry;

        Registry_unit_tests()
            : registry{maxEntities}
        {}

        ~Registry_unit_tests()
        {
            registry.Clear();
        }
};

TEST_F(Registry_unit_tests, AddEntity_ReturnsNonNull)
{
    auto handle = registry.Add<Entity>(TestInfo);
    EXPECT_TRUE(handle.Valid());
}

TEST_F(Registry_unit_tests, AddEntity_ConsecutiveCalls_ReturnsDistinctHandles)
{
    auto handle1 = registry.Add<Entity>(TestInfo);
    auto handle2 = registry.Add<Entity>(TestInfo);
    EXPECT_NE(handle1, handle2);
}

TEST_F(Registry_unit_tests, AddEntity_AddsTransform)
{
    auto handle = registry.Add<Entity>(TestInfo);
    EXPECT_TRUE(registry.Contains<Transform>(handle));
}

TEST_F(Registry_unit_tests, RemoveEntity_EntityExists_EntityRemovedFromActiveList)
{
    auto handle = registry.Add<Entity>(TestInfo);
    registry.CommitStagedChanges();
    auto entities = registry.ViewAll<Entity>();
    EXPECT_EQ(entities.size(), 1u);
    registry.Remove<Entity>(handle);
    entities = registry.ViewAll<Entity>();
    EXPECT_EQ(entities.size(), 0u);
}

TEST_F(Registry_unit_tests, RemoveEntity_DoesNotExist_Throws)
{
    EXPECT_THROW(registry.Remove<Entity>(Handle1), std::runtime_error);
}

TEST_F(Registry_unit_tests, ContainsEntity_Exists_ReturnsTrue)
{
    auto handle = registry.Add<Entity>(TestInfo);
    EXPECT_TRUE(registry.Contains<Entity>(handle));
}

TEST_F(Registry_unit_tests, ContainsEntity_DoesNotExist_ReturnsFalse)
{
    EXPECT_FALSE(registry.Contains<Entity>(Handle1));
}

TEST_F(Registry_unit_tests, ContainsEntity_AfterRemove_ReturnsFalse)
{
    auto handle = registry.Add<Entity>(TestInfo);
    registry.CommitStagedChanges();
    registry.Remove<Entity>(handle);
    EXPECT_FALSE(registry.Contains<Entity>(handle));
}

TEST_F(Registry_unit_tests, AddComponent_ValidCall_ConstructsObject)
{
    auto handle = registry.Add<Entity>({});
    auto* ptr = registry.Add<Fake1>(handle, 1);
    EXPECT_EQ(ptr->GetParentEntity(), handle);
    EXPECT_EQ(ptr->value, 1);
}

TEST_F(Registry_unit_tests, AddComponent_BadEntity_Throws)
{
    EXPECT_THROW(registry.Add<Fake1>(Entity{0u, 0u, 0u}, 1), std::runtime_error);
}

TEST_F(Registry_unit_tests, AddComponent_ReplaceAfterRemove_ConstructsObject)
{
    auto handle = registry.Add<Entity>({});
    registry.Add<Fake1>(handle, 1);
    registry.CommitStagedChanges();
    registry.Remove<Fake1>(handle);
    auto* ptr = registry.Add<Fake1>(handle, 2);
    EXPECT_EQ(ptr->GetParentEntity(), handle);
    EXPECT_EQ(ptr->value, 2);
}

TEST_F(Registry_unit_tests, AddComponent_DoubleCall_Throws)
{
    auto handle = registry.Add<Entity>({});
    registry.Add<Fake1>(handle, 1);
    EXPECT_THROW(registry.Add<Fake1>(handle, 1), std::runtime_error);
}

TEST_F(Registry_unit_tests, RemoveComponent_ComponentExists_Removes)
{
    auto handle = registry.Add<Entity>({});
    registry.Add<Fake1>(handle, 1);
    registry.CommitStagedChanges();
    registry.Remove<Fake1>(handle);
    auto actual = registry.Contains<Fake1>(handle);
    EXPECT_FALSE(actual);
}

TEST_F(Registry_unit_tests, RemoveComponent_BadEntity_Throws)
{
    EXPECT_THROW(registry.Remove<Fake1>(Entity{0u}), std::runtime_error);
}

TEST_F(Registry_unit_tests, RemoveComponent_ComponentDoesNotExist_Throws)
{
    auto handle = registry.Add<Entity>({});
    EXPECT_THROW(registry.Remove<Fake1>(handle), std::runtime_error);
}

TEST_F(Registry_unit_tests, RemoveComponent_DoubleCall_Throws)
{
    auto handle = registry.Add<Entity>({});
    registry.Add<Fake1>(handle, 1);
    registry.CommitStagedChanges();
    registry.Remove<Fake1>(handle);
    EXPECT_THROW(registry.Remove<Fake1>(handle), std::runtime_error);
}

TEST_F(Registry_unit_tests, RemoveComponent_AfterClear_Throws)
{
    auto handle = registry.Add<Entity>({});
    registry.Add<Fake1>(handle, 1);
    registry.Clear();
    EXPECT_THROW(registry.Remove<Fake1>(handle), std::runtime_error);
}

TEST_F(Registry_unit_tests, RemoveComponent_IncorrectType_Throws)
{
    auto handle = registry.Add<Entity>({});
    registry.Add<Fake1>(handle, 1);
    registry.CommitStagedChanges();
    EXPECT_THROW(registry.Remove<Fake2>(handle), std::runtime_error);
}

TEST_F(Registry_unit_tests, ContainsComponent_Exists_ReturnsTrue)
{
    auto handle = registry.Add<Entity>({});
    registry.Add<Fake1>(handle, 1);
    registry.CommitStagedChanges();
    auto actual = registry.Contains<Fake1>(handle);
    EXPECT_TRUE(actual);
}

TEST_F(Registry_unit_tests, ContainsComponent_BadEntity_Throws)
{
    EXPECT_THROW(registry.Contains<Fake1>(Entity{0u}), std::runtime_error);
}

TEST_F(Registry_unit_tests, ContainsComponent_ExistsStaged_ReturnsTrue)
{
    auto handle = registry.Add<Entity>({});
    registry.Add<Fake1>(handle, 1);
    auto actual = registry.Contains<Fake1>(handle);
    EXPECT_TRUE(actual);
}

TEST_F(Registry_unit_tests, ContainsComponent_DoesNotExist_ReturnsFalse)
{
    auto handle = registry.Add<Entity>({});
    auto actual = registry.Contains<Fake1>(handle);
    EXPECT_FALSE(actual);
}

TEST_F(Registry_unit_tests, ContainsComponent_AfterRemoved_ReturnsFalse)
{
    auto handle = registry.Add<Entity>({});
    registry.Add<Fake1>(handle, 1);
    registry.CommitStagedChanges();
    registry.Remove<Fake1>(handle);
    auto actual = registry.Contains<Fake1>(handle);
    EXPECT_FALSE(actual);
}

TEST_F(Registry_unit_tests, GetComponent_Exists_ReturnsPointer)
{
    auto handle = registry.Add<Entity>({});
    auto value = 10;
    registry.Add<Fake1>(handle, value);
    registry.CommitStagedChanges();
    auto* ptr = registry.Get<Fake1>(handle);
    ASSERT_NE(ptr, nullptr);
    EXPECT_EQ(ptr->value, value);
}

TEST_F(Registry_unit_tests, GetComponent_BadEntity_ReturnsNull)
{
    auto* actual = registry.Get<Fake1>(Entity{0u});
    EXPECT_EQ(actual, nullptr);
}

TEST_F(Registry_unit_tests, GetComponent_ExistsStaged_ReturnsPointer)
{
    auto handle = registry.Add<Entity>({});
    auto value = 10;
    registry.Add<Fake1>(handle, value);
    auto* ptr = registry.Get<Fake1>(handle);
    ASSERT_NE(ptr, nullptr);
    EXPECT_EQ(ptr->value, value);
}

TEST_F(Registry_unit_tests, GetComponent_DoesNotExist_ReturnsNull)
{
    auto handle = registry.Add<Entity>({});
    auto* ptr = registry.Get<Fake1>(handle);
    EXPECT_EQ(ptr, nullptr);
}

TEST_F(Registry_unit_tests, GetComponent_CallAfterRemoved_ReturnsNull)
{
    auto handle = registry.Add<Entity>({});
    registry.Add<Fake1>(handle, 1);
    registry.CommitStagedChanges();
    registry.Remove<Fake1>(handle);
    auto* ptr = registry.Get<Fake1>(handle);
    EXPECT_EQ(ptr, nullptr);
}

TEST_F(Registry_unit_tests, GetComponentConst_Exists_ReturnsPointer)
{
    auto handle = registry.Add<Entity>({});
    auto value = 10;
    registry.Add<Fake1>(handle, value);
    registry.CommitStagedChanges();
    const auto& constRegistry = registry;
    const auto* ptr = constRegistry.Get<Fake1>(handle);
    ASSERT_NE(ptr, nullptr);
    EXPECT_EQ(ptr->value, value);
}

TEST_F(Registry_unit_tests, GetComponentConst_BadEntity_ReturnsNull)
{
    const auto& constRegistry = registry;
    const auto* actual = constRegistry.Get<Fake1>(Entity{0u});
    EXPECT_EQ(actual, nullptr);
}

TEST_F(Registry_unit_tests, GetComponentConst_ExistsStaged_ReturnsPointer)
{
    auto handle = registry.Add<Entity>({});
    auto value = 10;
    registry.Add<Fake1>(handle, value);
    const auto& constRegistry = registry;
    const auto* ptr = constRegistry.Get<Fake1>(handle);
    ASSERT_NE(ptr, nullptr);
    EXPECT_EQ(ptr->value, value);
}

TEST_F(Registry_unit_tests, GetComponentConst_DoesNotExist_ReturnsNull)
{
    auto handle = registry.Add<Entity>({});
    const auto& constRegistry = registry;
    const auto* ptr = constRegistry.Get<Fake1>(handle);
    EXPECT_EQ(ptr, nullptr);
}

TEST_F(Registry_unit_tests, GetComponentConst_CallAfterRemoved_ReturnsNull)
{
    auto handle = registry.Add<Entity>({});
    registry.Add<Fake1>(handle, 1);
    registry.CommitStagedChanges();
    registry.Remove<Fake1>(handle);
    const auto& constRegistry = registry;
    const auto* ptr = constRegistry.Get<Fake1>(handle);
    EXPECT_EQ(ptr, nullptr);
}

TEST_F(Registry_unit_tests, AddAutoComponent_ValidCall_ConstructsObject)
{
    auto handle = registry.Add<Entity>({});
    auto* ptr = registry.Add<FakeAutoComponent>(handle, 1);
    EXPECT_EQ(ptr->GetParentEntity(), handle);
    EXPECT_EQ(ptr->value, 1);
}

TEST_F(Registry_unit_tests, AddAutoComponent_BadEntity_Throws)
{
    EXPECT_THROW(registry.Add<FakeAutoComponent>(Entity{0u}, 1), std::runtime_error);
}

TEST_F(Registry_unit_tests, AddAutoComponent_ReplaceAfterRemove_ConstructsObject)
{
    auto handle = registry.Add<Entity>({});
    registry.Add<FakeAutoComponent>(handle, 1);
    registry.CommitStagedChanges();
    registry.Remove<FakeAutoComponent>(handle);
    auto* ptr = registry.Add<FakeAutoComponent>(handle, 2);
    EXPECT_EQ(ptr->GetParentEntity(), handle);
    EXPECT_EQ(ptr->value, 2);
}

TEST_F(Registry_unit_tests, AddAutoComponent_DoubleCall_Throws)
{
    auto handle = registry.Add<Entity>({});
    registry.Add<FakeAutoComponent>(handle, 1);
    EXPECT_THROW(registry.Add<FakeAutoComponent>(handle, 1), std::runtime_error);
}

TEST_F(Registry_unit_tests, RemoveAutoComponent_ComponentExists_Removes)
{
    auto handle = registry.Add<Entity>({});
    registry.Add<FakeAutoComponent>(handle, 1);
    registry.CommitStagedChanges();
    registry.Remove<FakeAutoComponent>(handle);
    auto actual = registry.Contains<FakeAutoComponent>(handle);
    EXPECT_FALSE(actual);
}

TEST_F(Registry_unit_tests, RemoveAutoComponent_BadEntity_Throws)
{
    EXPECT_THROW(registry.Remove<FakeAutoComponent>(Entity{0u}), std::runtime_error);
}

TEST_F(Registry_unit_tests, RemoveAutoComponent_ComponentDoesNotExist_Throws)
{
    auto handle = registry.Add<Entity>({});
    EXPECT_THROW(registry.Remove<FakeAutoComponent>(handle), std::runtime_error);
}

TEST_F(Registry_unit_tests, RemoveAutoComponent_DoubleCall_Throws)
{
    auto handle = registry.Add<Entity>({});
    registry.Add<FakeAutoComponent>(handle, 1);
    registry.CommitStagedChanges();
    registry.Remove<FakeAutoComponent>(handle);
    EXPECT_THROW(registry.Remove<FakeAutoComponent>(handle), std::runtime_error);
}

TEST_F(Registry_unit_tests, RemoveAutoComponent_AfterClear_Throws)
{
    auto handle = registry.Add<Entity>({});
    registry.Add<FakeAutoComponent>(handle, 1);
    registry.CommitStagedChanges();
    registry.Clear();
    EXPECT_THROW(registry.Remove<FakeAutoComponent>(handle), std::runtime_error);
}

TEST_F(Registry_unit_tests, ContainsAutoComponent_Exists_ReturnsTrue)
{
    auto handle = registry.Add<Entity>({});
    registry.Add<FakeAutoComponent>(handle, 1);
    registry.CommitStagedChanges();
    auto actual = registry.Contains<FakeAutoComponent>(handle);
    EXPECT_TRUE(actual);
}

TEST_F(Registry_unit_tests, ContainsAutoComponent_BadEntity_Throws)
{
    EXPECT_THROW(registry.Contains<FakeAutoComponent>(Entity{0u}), std::runtime_error);
}

TEST_F(Registry_unit_tests, ContainsAutoComponent_ExistsStaged_ReturnsTrue)
{
    auto handle = registry.Add<Entity>({});
    registry.Add<FakeAutoComponent>(handle, 1);
    auto actual = registry.Contains<FakeAutoComponent>(handle);
    EXPECT_TRUE(actual);
}

TEST_F(Registry_unit_tests, ContainsAutoComponent_DoesNotExist_ReturnsFalse)
{
    auto handle = registry.Add<Entity>({});
    auto actual = registry.Contains<FakeAutoComponent>(handle);
    EXPECT_FALSE(actual);
}

TEST_F(Registry_unit_tests, ContainsAutoComponent_AfterRemoved_ReturnsFalse)
{
    auto handle = registry.Add<Entity>({});
    registry.Add<FakeAutoComponent>(handle, 1);
    registry.CommitStagedChanges();
    registry.Remove<FakeAutoComponent>(handle);
    auto actual = registry.Contains<FakeAutoComponent>(handle);
    EXPECT_FALSE(actual);
}

TEST_F(Registry_unit_tests, GetAutoComponent_Exists_ReturnsPointer)
{
    auto handle = registry.Add<Entity>({});
    auto value = 10;
    registry.Add<FakeAutoComponent>(handle, value);
    registry.CommitStagedChanges();
    auto* ptr = registry.Get<FakeAutoComponent>(handle);
    ASSERT_NE(ptr, nullptr);
    EXPECT_EQ(ptr->value, value);
}

TEST_F(Registry_unit_tests, GetAutoComponent_BadEntity_Throws)
{
    auto* actual = registry.Get<FakeAutoComponent>(Entity{0u});
    EXPECT_EQ(actual, nullptr);
}

TEST_F(Registry_unit_tests, GetAutoComponent_ExistsStaged_ReturnsPointer)
{
    auto handle = registry.Add<Entity>({});
    auto value = 10;
    registry.Add<FakeAutoComponent>(handle, value);
    auto* ptr = registry.Get<FakeAutoComponent>(handle);
    ASSERT_NE(ptr, nullptr);
    EXPECT_EQ(ptr->value, value);
}

TEST_F(Registry_unit_tests, GetAutoComponent_DoesNotExist_ReturnsNull)
{
    auto handle = registry.Add<Entity>({});
    auto* ptr = registry.Get<FakeAutoComponent>(handle);
    EXPECT_EQ(ptr, nullptr);
}

TEST_F(Registry_unit_tests, GetAutoComponent_CallAfterRemoved_ReturnsNull)
{
    auto handle = registry.Add<Entity>({});
    registry.Add<FakeAutoComponent>(handle, 1);
    registry.CommitStagedChanges();
    registry.Remove<FakeAutoComponent>(handle);
    auto* ptr = registry.Get<FakeAutoComponent>(handle);
    EXPECT_EQ(ptr, nullptr);
}

TEST_F(Registry_unit_tests, GetAutoComponentConst_Exists_ReturnsPointer)
{
    auto handle = registry.Add<Entity>({});
    auto value = 10;
    registry.Add<FakeAutoComponent>(handle, value);
    registry.CommitStagedChanges();
    const auto& constRegistry = registry;
    const auto* ptr = constRegistry.Get<FakeAutoComponent>(handle);
    ASSERT_NE(ptr, nullptr);
    EXPECT_EQ(ptr->value, value);
}

TEST_F(Registry_unit_tests, GetAutoComponentConst_BadEntity_ReturnsNull)
{
    const auto& constRegistry = registry;
    const auto* actual = constRegistry.Get<FakeAutoComponent>(Entity{8u});
    EXPECT_EQ(actual, nullptr);
}

TEST_F(Registry_unit_tests, GetAutoComponentConst_ExistsStaged_ReturnsPointer)
{
    auto handle = registry.Add<Entity>({});
    auto value = 10;
    registry.Add<FakeAutoComponent>(handle, value);
    const auto& constRegistry = registry;
    const auto* ptr = constRegistry.Get<FakeAutoComponent>(handle);
    ASSERT_NE(ptr, nullptr);
    EXPECT_EQ(ptr->value, value);
}

TEST_F(Registry_unit_tests, GetAutoComponentConst_DoesNotExist_ReturnsNull)
{
    auto handle = registry.Add<Entity>({});
    const auto& constRegistry = registry;
    const auto* ptr = constRegistry.Get<FakeAutoComponent>(handle);
    EXPECT_EQ(ptr, nullptr);
}

TEST_F(Registry_unit_tests, GetAutoComponentConst_CallAfterRemoved_ReturnsNull)
{
    auto handle = registry.Add<Entity>({});
    registry.Add<FakeAutoComponent>(handle, 1);
    registry.CommitStagedChanges();
    registry.Remove<FakeAutoComponent>(handle);
    const auto& constRegistry = registry;
    const auto* ptr = constRegistry.Get<FakeAutoComponent>(handle);
    EXPECT_EQ(ptr, nullptr);
}

TEST_F(Registry_unit_tests, ViewGroup_FirstGroupLarger_ReturnsSortedViews)
{
    auto h1 = registry.Add<Entity>({});
    auto h2 = registry.Add<Entity>({});
    auto h3 = registry.Add<Entity>({});
    auto h4 = registry.Add<Entity>({});
    auto h5 = registry.Add<Entity>({});

    registry.Add<Fake1>(h2, 1);
    registry.Add<Fake1>(h1, 1);
    registry.Add<Fake1>(h3, 1);
    registry.Add<Fake1>(h5, 1);
    registry.Add<Fake1>(h4, 1);

    registry.Add<Fake2>(h5, 1);
    registry.Add<Fake2>(h4, 1);
    registry.Add<Fake2>(h1, 1);

    registry.CommitStagedChanges();

    auto [fake1, fake2] = registry.ViewGroup<Fake1, Fake2>();

    auto fake1Size = fake1.size();
    auto fake2Size = fake2.size();
    ASSERT_EQ(fake1Size, fake2Size);
    EXPECT_EQ(fake1Size, 3u);

    for(size_t i = 0u; i < fake1Size; ++i)
    {
        EXPECT_EQ(fake1[i].GetParentEntity(), fake2[i].GetParentEntity());
    }
}

TEST_F(Registry_unit_tests, ViewGroup_SecondGroupLarger_ReturnsSortedViews)
{
    auto h1 = registry.Add<Entity>({});
    auto h2 = registry.Add<Entity>({});
    auto h3 = registry.Add<Entity>({});
    auto h4 = registry.Add<Entity>({});
    auto h5 = registry.Add<Entity>({});

    registry.Add<Fake1>(h1, 1);
    registry.Add<Fake1>(h2, 1);
    registry.Add<Fake1>(h3, 1);

    registry.Add<Fake2>(h5, 1);
    registry.Add<Fake2>(h4, 1);
    registry.Add<Fake2>(h1, 1);
    registry.Add<Fake2>(h2, 1);
    registry.Add<Fake2>(h3, 1);

    registry.CommitStagedChanges();

    auto [fake1, fake2] = registry.ViewGroup<Fake1, Fake2>();

    auto fake1Size = fake1.size();
    auto fake2Size = fake2.size();
    ASSERT_EQ(fake1Size, fake2Size);
    EXPECT_EQ(fake1Size, 3u);

    for(size_t i = 0u; i < fake1Size; ++i)
    {
        EXPECT_EQ(fake1[i].GetParentEntity(), fake2[i].GetParentEntity());
    }
}

int main(int argc, char ** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}