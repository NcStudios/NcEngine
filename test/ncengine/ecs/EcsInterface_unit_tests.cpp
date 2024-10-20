#include "gtest/gtest.h"
#include "ncengine/ecs/Ecs.h"

struct S1 {};
struct S2 {};

struct TestFreeComponent : public nc::FreeComponent
{
    TestFreeComponent(nc::Entity e) : nc::FreeComponent{e} {}
};

class EcsInterfaceTests : public ::testing::Test
{
    public:
        static constexpr size_t registryCapacity = 10ull;
        nc::ecs::ComponentRegistry registry;

        EcsInterfaceTests()
            : registry{registryCapacity}
        {
            registry.RegisterType<nc::Tag>(10);
            registry.RegisterType<nc::Transform>(10);
            registry.RegisterType<nc::ecs::detail::FreeComponentGroup>(10);
            registry.RegisterType<nc::Hierarchy>(10);
            registry.RegisterType<S1>(10);
            registry.RegisterType<S2>(10);
        }

        // Mock of EcsModule for committing registry changes
        void TestFixture_SyncRegistry()
        {
            registry.CommitPendingChanges();
            for(auto& bag : registry.GetPool<nc::ecs::detail::FreeComponentGroup>())
                bag.CommitStagedComponents();
        }
};

template<class FilteredEcs, class T>
concept CanAccess = requires (FilteredEcs ecs, nc::Entity entity)
{
    { ecs.template Contains<T>(entity) } -> std::same_as<bool>;
};

TEST_F(EcsInterfaceTests, Aliases_includesExpectedTypes)
{
    using all = nc::ecs::Ecs; // no type restrictions
    static_assert(CanAccess<all, nc::Entity>);
    static_assert(CanAccess<all, nc::Tag>);
    static_assert(CanAccess<all, nc::Transform>);
    static_assert(CanAccess<all, nc::Hierarchy>);
    static_assert(CanAccess<all, nc::ecs::detail::FreeComponentGroup>);
    static_assert(CanAccess<all, S1>);
    static_assert(CanAccess<all, S2>);

    using basic = nc::ecs::BasicEcs<S1, S2>; // includes basic types + explicit type list
    static_assert(CanAccess<basic, nc::Entity>);
    static_assert(CanAccess<basic, nc::Tag>);
    static_assert(CanAccess<basic, nc::Transform>);
    static_assert(CanAccess<basic, nc::Hierarchy>);
    static_assert(CanAccess<basic, nc::ecs::detail::FreeComponentGroup>);
    static_assert(CanAccess<basic, S1>);
    static_assert(CanAccess<basic, S2>);

    using strict = nc::ecs::ExplicitEcs<S1>; // includes only FreeComponent + explicit type list
    static_assert(!CanAccess<strict, nc::Entity>);
    static_assert(!CanAccess<strict, nc::Tag>);
    static_assert(!CanAccess<strict, nc::Transform>);
    static_assert(!CanAccess<strict, nc::Hierarchy>);
    static_assert(CanAccess<strict, nc::ecs::detail::FreeComponentGroup>);
    static_assert(CanAccess<strict, S1>);
    static_assert(!CanAccess<strict, S2>);
}

TEST_F(EcsInterfaceTests, ConversionOperator_onlyValidWhenMoreRestrictive)
{
    using all = nc::ecs::Ecs;
    using customBasic = nc::ecs::BasicEcs<S1>;
    using stockBasic = nc::ecs::BasicEcs<>;
    using strict = nc::ecs::ExplicitEcs<nc::Entity>;
    using none = nc::ecs::ExplicitEcs<>;

    static_assert(std::constructible_from<customBasic, all>);
    static_assert(std::constructible_from<stockBasic, customBasic>);
    static_assert(std::constructible_from<stockBasic, customBasic>);
    static_assert(std::constructible_from<strict, stockBasic>);
    static_assert(std::constructible_from<none, strict>);

    static_assert(!std::constructible_from<all, customBasic>);
    static_assert(!std::constructible_from<customBasic, stockBasic>);
    static_assert(!std::constructible_from<stockBasic, strict>);
    static_assert(!std::constructible_from<strict, none>);
}

TEST_F(EcsInterfaceTests, Emplace_entity_addsExpectedState)
{
    auto uut = nc::ecs::Ecs{registry};
    const auto e = uut.Emplace<nc::Entity>();
    EXPECT_TRUE(e.Valid());
    EXPECT_TRUE(uut.Contains<nc::Entity>(e));
    EXPECT_TRUE(uut.Contains<nc::Tag>(e));
    EXPECT_TRUE(uut.Contains<nc::Transform>(e));
    EXPECT_TRUE(uut.Contains<nc::ecs::detail::FreeComponentGroup>(e));
    EXPECT_TRUE(uut.Contains<nc::Hierarchy>(e));
}

TEST_F(EcsInterfaceTests, Emplace_entity_setsHierarchy)
{
    auto uut = nc::ecs::Ecs{registry};
    const auto e1 = uut.Emplace<nc::Entity>();
    const auto e2 = uut.Emplace<nc::Entity>({.parent = e1});
    const auto& h1 = uut.Get<nc::Hierarchy>(e1);
    const auto& h2 = uut.Get<nc::Hierarchy>(e2);
    EXPECT_EQ(nc::Entity::Null(), h1.parent);
    ASSERT_EQ(1, h1.children.size());
    EXPECT_EQ(e2, h1.children[0]);
    EXPECT_EQ(e1, h2.parent);
    EXPECT_EQ(0, h2.children.size());
}

TEST_F(EcsInterfaceTests, Emplace_component_addsComponent)
{
    static_assert(nc::PooledComponent<S1>);
    auto uut = nc::ecs::Ecs{registry};
    const auto good = uut.Emplace<nc::Entity>();
    const auto bad = nc::Entity{3, 0, 0};
    uut.Emplace<S1>(good);
    EXPECT_THROW(uut.Emplace<S1>(bad), nc::NcError);
    EXPECT_TRUE(uut.Contains<S1>(good));
    EXPECT_FALSE(uut.Contains<S1>(bad));
}

TEST_F(EcsInterfaceTests, Emplace_freeComponent_addsItem)
{
    auto uut = nc::ecs::Ecs{registry};
    const auto entity = uut.Emplace<nc::Entity>();
    uut.Emplace<TestFreeComponent>(entity);
    EXPECT_TRUE(uut.Contains<TestFreeComponent>(entity));
}

TEST_F(EcsInterfaceTests, Remove_entity_happyAndErrorPathsSucceed)
{
    auto uut = nc::ecs::Ecs{registry};
    const auto entity = uut.Emplace<nc::Entity>();
    uut.Emplace<S1>(entity);
    EXPECT_TRUE(uut.Remove<nc::Entity>(entity));
    EXPECT_FALSE(uut.Contains<nc::Entity>(entity));
    EXPECT_FALSE(uut.Remove<nc::Entity>(entity));
    registry.CommitPendingChanges();
    EXPECT_FALSE(uut.Contains<S1>(entity));
}

TEST_F(EcsInterfaceTests, Remove_entity_updatesHierarchy)
{
    auto uut = nc::ecs::Ecs{registry};
    const auto e1 = uut.Emplace<nc::Entity>();
    const auto e2 = uut.Emplace<nc::Entity>({.parent = e1});
    EXPECT_TRUE(uut.Remove<nc::Entity>(e2));
    EXPECT_EQ(0, uut.Get<nc::Hierarchy>(e1).children.size());
}

TEST_F(EcsInterfaceTests, Remove_component_happyAndErrorPathsSucceed)
{
    auto uut = nc::ecs::Ecs{registry};
    const auto entity = uut.Emplace<nc::Entity>();
    uut.Emplace<S1>(entity);
    EXPECT_TRUE(uut.Remove<S1>(entity));
    EXPECT_FALSE(uut.Contains<S1>(entity));
    registry.CommitPendingChanges(); // verify things are well-behaved during merge
    EXPECT_FALSE(uut.Contains<S1>(entity));
}

TEST_F(EcsInterfaceTests, Remove_freeComponent_happyAndErrorPathsSucceed)
{
    auto uut = nc::ecs::Ecs{registry};
    const auto entity = uut.Emplace<nc::Entity>();
    uut.Emplace<TestFreeComponent>(entity);
    TestFixture_SyncRegistry();
    EXPECT_TRUE(uut.Remove<TestFreeComponent>(entity));
    TestFixture_SyncRegistry();
    EXPECT_FALSE(uut.Contains<TestFreeComponent>(entity));
    EXPECT_FALSE(uut.Remove<TestFreeComponent>(entity));
}

TEST_F(EcsInterfaceTests, Get_component_happyPath_succeeds)
{
    auto uut = nc::ecs::Ecs{registry};
    const auto entity = uut.Emplace<nc::Entity>();
    const auto& expected = uut.Emplace<S1>(entity);
    EXPECT_EQ(&expected, &uut.Get<S1>(entity));
}

TEST_F(EcsInterfaceTests, Get_component_errorPath_throws)
{
    auto uut = nc::ecs::Ecs{registry};
    const auto entity = uut.Emplace<nc::Entity>();
    uut.Emplace<S1>(entity);
    uut.Remove<S1>(entity);
    EXPECT_THROW(uut.Get<S1>(entity), nc::NcError);
    const auto badEntity = nc::Entity{2, 0, 0};
    EXPECT_THROW(uut.Get<S1>(badEntity), nc::NcError);
}

TEST_F(EcsInterfaceTests, Get_freeComponent_happyPath_succeed)
{
    auto uut = nc::ecs::Ecs{registry};
    const auto entity = uut.Emplace<nc::Entity>();
    const auto& expected = uut.Emplace<TestFreeComponent>(entity);
    EXPECT_EQ(&expected, &uut.Get<TestFreeComponent>(entity));
}

TEST_F(EcsInterfaceTests, Get_freeComponent_errorPath_throws)
{
    auto uut = nc::ecs::Ecs{registry};
    const auto entity = uut.Emplace<nc::Entity>();
    uut.Emplace<TestFreeComponent>(entity);
    TestFixture_SyncRegistry();
    uut.Remove<TestFreeComponent>(entity);
    TestFixture_SyncRegistry();
    EXPECT_THROW(uut.Get<TestFreeComponent>(entity), nc::NcError);
    const auto badEntity = nc::Entity{2, 0, 0};
    EXPECT_THROW(uut.Get<TestFreeComponent>(badEntity), nc::NcError);
}

TEST_F(EcsInterfaceTests, GetEntityByTag_tagExists_returnsEntity)
{
    constexpr auto tag = "FindMe";
    auto uut = nc::ecs::Ecs{registry};
    const auto expected = uut.Emplace<nc::Entity>({.tag = tag});
    registry.CommitPendingChanges();
    const auto actual = uut.GetEntityByTag(tag);
    EXPECT_EQ(expected, actual);
}

TEST_F(EcsInterfaceTests, GetEntityByTag_multipleMatchingTags_returnsFirst)
{
    constexpr auto tag = "ManyOf";
    auto uut = nc::ecs::Ecs{registry};
    const auto expected = uut.Emplace<nc::Entity>({.tag = tag});
    uut.Emplace<nc::Entity>({.tag = tag});
    uut.Emplace<nc::Entity>({.tag = tag});
    registry.CommitPendingChanges();
    const auto actual = uut.GetEntityByTag(tag);
    EXPECT_EQ(expected, actual);
}

TEST_F(EcsInterfaceTests, GetEntityByTag_tagDoesNotExist_throws)
{
    auto uut = nc::ecs::Ecs{registry};
    uut.Emplace<nc::Entity>({.tag = "Unreachable"});
    registry.CommitPendingChanges();
    EXPECT_THROW(uut.GetEntityByTag("NoneSuch"), nc::NcError);
}

TEST_F(EcsInterfaceTests, GetAll_entity_returnsAllEntities)
{
    auto uut = nc::ecs::Ecs{registry};
    auto expected = std::vector<nc::Entity>{};
    expected.push_back(uut.Emplace<nc::Entity>());
    const auto toRemove = uut.Emplace<nc::Entity>();
    uut.Remove<nc::Entity>(toRemove);
    expected.push_back(uut.Emplace<nc::Entity>());

    const auto actual = uut.GetAll<nc::Entity>();
    ASSERT_EQ(expected.size(), actual.size());
    EXPECT_TRUE(std::ranges::equal(expected, actual));

    const auto& uutConst = uut;
    const auto actualConst = uutConst.GetAll<nc::Entity>();
    ASSERT_EQ(expected.size(), actualConst.size());
    EXPECT_TRUE(std::ranges::equal(expected, actualConst));
}

TEST_F(EcsInterfaceTests, GetAll_component_returnsAllTs)
{
    auto uut = nc::ecs::Ecs{registry};
    const auto& notRemoved = uut.Emplace<nc::Entity>();
    const auto& indirectlyRemoved = uut.Emplace<nc::Entity>();
    const auto& directlyRemoved = uut.Emplace<nc::Entity>();
    uut.Emplace<S1>(notRemoved);
    uut.Emplace<S1>(indirectlyRemoved);
    uut.Emplace<S1>(directlyRemoved);
    TestFixture_SyncRegistry();
    uut.Remove<nc::Entity>(indirectlyRemoved); // indirect removal requires registry sync; still expect to see
    uut.Remove<S1>(directlyRemoved); // direct removal is immediate; do not expect to see
    const auto& staged = uut.Emplace<nc::Entity>();
    uut.Emplace<S1>(staged); // staged until registry sync; do not expect to see

    const auto expected = std::vector<nc::Entity>{notRemoved, indirectlyRemoved};
    const auto actual = uut.GetAll<S1>();
    ASSERT_EQ(2, actual.size());
    EXPECT_EQ(notRemoved, uut.GetParent(&actual[0]));
    EXPECT_EQ(indirectlyRemoved, uut.GetParent(&actual[1]));

    const auto& uutConst = uut;
    const auto actualConst = uutConst.GetAll<S1>();
    ASSERT_EQ(2, actualConst.size());
}

TEST_F(EcsInterfaceTests, SetParent_makeChild_updatesHierarchies)
{
    auto uut = nc::ecs::Ecs{registry};
    const auto parent = uut.Emplace<nc::Entity>();
    const auto child = uut.Emplace<nc::Entity>();
    uut.SetParent(child, parent);
    const auto& parentHierarchy = uut.Get<nc::Hierarchy>(parent);
    const auto& childHierarchy = uut.Get<nc::Hierarchy>(child);
    ASSERT_EQ(1, parentHierarchy.children.size());
    EXPECT_EQ(child, parentHierarchy.children[0]);
    EXPECT_EQ(parent, childHierarchy.parent);
}

TEST_F(EcsInterfaceTests, SetParent_makeRoot_updatesHierarchies)
{
    auto uut = nc::ecs::Ecs{registry};
    const auto parent = uut.Emplace<nc::Entity>();
    const auto child = uut.Emplace<nc::Entity>({.parent = parent});
    uut.SetParent(child, nc::Entity::Null());
    const auto& parentHierarchy = uut.Get<nc::Hierarchy>(parent);
    const auto& childHierarchy = uut.Get<nc::Hierarchy>(child);
    EXPECT_EQ(0, parentHierarchy.children.size());
    EXPECT_EQ(nc::Entity::Null(), childHierarchy.parent);
}

TEST_F(EcsInterfaceTests, GetRoot_traversesHierarchy)
{
    auto uut = nc::ecs::Ecs{registry};
    const auto loneRoot = uut.Emplace<nc::Entity>();
    const auto root = uut.Emplace<nc::Entity>();
    const auto middle1 = uut.Emplace<nc::Entity>({.parent = root});
    const auto middle2 = uut.Emplace<nc::Entity>({.parent = root});
    const auto leaf = uut.Emplace<nc::Entity>({.parent = middle1});
    const auto actualRootOfLoneRoot = uut.GetRoot(loneRoot);
    const auto actualRootOfRoot = uut.GetRoot(root);
    const auto actualRootOfMiddle1 = uut.GetRoot(middle1);
    const auto actualRootOfMiddle2 = uut.GetRoot(middle2);
    const auto actualRootOfLeaf = uut.GetRoot(leaf);
    EXPECT_EQ(loneRoot, actualRootOfLoneRoot);
    EXPECT_EQ(root, actualRootOfRoot);
    EXPECT_EQ(root, actualRootOfMiddle1);
    EXPECT_EQ(root, actualRootOfMiddle2);
    EXPECT_EQ(root, actualRootOfLeaf);
}

TEST_F(EcsInterfaceTests, GetParent_happyAndErrorPathsSucceed)
{
    auto uut = nc::ecs::Ecs{registry};
    const auto& expected = uut.Emplace<nc::Entity>();
    const auto& stagedComponent = uut.Emplace<S1>(expected);
    EXPECT_EQ(expected, uut.GetParent(&stagedComponent));
    TestFixture_SyncRegistry();
    const auto& mergedComponent = uut.Get<S1>(expected);
    EXPECT_EQ(expected, uut.GetParent(&mergedComponent));
    uut.Remove<S1>(expected);
    EXPECT_EQ(nc::Entity::Null(), uut.GetParent(&mergedComponent));
    TestFixture_SyncRegistry();
    EXPECT_EQ(nc::Entity::Null(), uut.GetParent(&mergedComponent));
    uut.Remove<nc::Entity>(expected);
    TestFixture_SyncRegistry();
    EXPECT_EQ(nc::Entity::Null(), uut.GetParent(&mergedComponent));
}
