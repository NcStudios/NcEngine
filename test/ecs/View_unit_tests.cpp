#include "gtest/gtest.h"
#include "ecs/View.h"

#include <iterator>
#include <ranges>

using namespace nc;

namespace nc
{
Quaternion::Quaternion(float X, float Y, float Z, float W)
    : x{X}, y{Y}, z{Z}, w{W}
{}

namespace ecs::detail
{
void FreeComponentGroup::CommitStagedComponents() {}
}
}

struct Fake1 : public ComponentBase
{
    Fake1(Entity entity, int v) : ComponentBase{entity}, value{v} {}
    int value;
};

struct Fake2 : public ComponentBase
{
    Fake2(Entity entity, int v) : ComponentBase{entity}, value{v} {}
    int value;
};

struct Fake3 : public ComponentBase
{
    Fake3(Entity entity, int v) : ComponentBase{entity}, value{v} {}
    int value;
};

class View_unit_tests : public ::testing::Test
{
    public:
        static inline ecs::ComponentRegistry impl{10u};
        static inline Registry registry{impl};

        static void SetUpTestSuite()
        {
            impl.RegisterType<nc::Tag>(10);
            impl.RegisterType<nc::Transform>(10);
            impl.RegisterType<nc::ecs::detail::FreeComponentGroup>(10);
            impl.RegisterType<nc::Hierarchy>(10);
            impl.RegisterType<Fake1>(10);
            impl.RegisterType<Fake2>(10);
            impl.RegisterType<Fake3>(10);
        }

        ~View_unit_tests()
        {
            registry.CommitStagedChanges();
            registry.Clear();
        }
};

TEST_F(View_unit_tests, TypeAssertions)
{
    using view_t = nc::ecs::InnerJoinView<nc::ecs::Ecs, Fake1, Fake2>;
    using iterator_t = decltype(std::declval<view_t>().begin());
    using sentinel_t = decltype(std::declval<view_t>().end());

    static_assert(std::forward_iterator<iterator_t>);
    static_assert(std::sentinel_for<sentinel_t, iterator_t>);
    static_assert(std::ranges::forward_range<view_t>);
    static_assert(std::ranges::view<view_t>);
}

TEST_F(View_unit_tests, InnerJoin_singleEntity_returnsComponentSet)
{
    auto ecs = nc::ecs::Ecs{impl};
    auto e1 = ecs.Emplace<Entity>(EntityInfo{});
    ecs.Emplace<Fake1>(e1, 0);
    ecs.Emplace<Fake2>(e1, 1);
    ecs.Emplace<Fake3>(e1, 2);
    impl.CommitPendingChanges();

    auto uut = nc::ecs::InnerJoin<Fake1, Fake2, Fake3>(ecs, e1);
    auto [fake1, fake2, fake3] = uut;
    ASSERT_NE(nullptr, fake1);
    ASSERT_NE(nullptr, fake2);
    ASSERT_NE(nullptr, fake3);
    EXPECT_EQ(e1, fake1->ParentEntity());
    EXPECT_EQ(e1, fake2->ParentEntity());
    EXPECT_EQ(e1, fake3->ParentEntity());
    EXPECT_EQ(0, fake1->value);
    EXPECT_EQ(1, fake2->value);
    EXPECT_EQ(2, fake3->value);
}

TEST_F(View_unit_tests, InnerJoin_singleType_returnsComponentRange)
{
    auto ecs = nc::ecs::Ecs{impl};

    {
        auto uut = nc::ecs::InnerJoin<Fake1>(ecs);
        EXPECT_EQ(0ull, uut.size_upper_bound());
        EXPECT_EQ(uut.begin(), uut.end());
        EXPECT_NO_THROW(std::ranges::for_each(uut, [](auto&&){}));
    }

    auto e1 = ecs.Emplace<Entity>(EntityInfo{});
    auto e2 = ecs.Emplace<Entity>(EntityInfo{});
    auto e3 = ecs.Emplace<Entity>(EntityInfo{});
    ecs.Emplace<Fake1>(e1, 0);
    ecs.Emplace<Fake1>(e2, 1);
    ecs.Emplace<Fake1>(e3, 2);
    impl.CommitPendingChanges();

    auto uut = nc::ecs::InnerJoin<Fake1>(ecs);
    EXPECT_EQ(3ull, uut.size_upper_bound());
    EXPECT_NE(uut.begin(), uut.end());
    for (const auto& [fake] : uut)
    {
        EXPECT_NE(nullptr, fake);
    }
}

TEST_F(View_unit_tests, InnerJoin_multipleTypes_returnsComponentRange)
{
    auto ecs = nc::ecs::Ecs{impl};

    {
        auto uut = nc::ecs::InnerJoin<Fake1, Fake2>(ecs);
        EXPECT_FALSE(uut);
        EXPECT_TRUE(uut.empty());
        EXPECT_EQ(0ull, uut.size_upper_bound());
        EXPECT_EQ(uut.begin(), uut.end());
        EXPECT_EQ(uut.cbegin(), uut.cend());
        EXPECT_NO_THROW(std::ranges::for_each(uut, [](auto&&){}));
    }

    auto e1 = ecs.Emplace<Entity>(EntityInfo{});
    auto e2 = ecs.Emplace<Entity>(EntityInfo{});
    auto e3 = ecs.Emplace<Entity>(EntityInfo{});
    ecs.Emplace<Fake1>(e1, 0);
    ecs.Emplace<Fake1>(e2, 1);
    ecs.Emplace<Fake1>(e3, 2);
    ecs.Emplace<Fake2>(e1, 3);
    ecs.Emplace<Fake2>(e2, 4);
    impl.CommitPendingChanges();

    auto uut = nc::ecs::InnerJoin<Fake1, Fake2>(ecs);
    EXPECT_TRUE(uut);
    EXPECT_FALSE(uut.empty());
    EXPECT_EQ(2ull, uut.size_upper_bound());
    EXPECT_NE(uut.begin(), uut.end());
    EXPECT_NE(uut.cbegin(), uut.cend());
    for (const auto& [fake1, fake2] : uut)
    {
        ASSERT_NE(nullptr, fake1);
        ASSERT_NE(nullptr, fake2);
    }

    {
        const auto& [frontFake1, frontFake2] = uut.front();
        EXPECT_EQ(0, frontFake1->value);
        EXPECT_EQ(3, frontFake2->value);
    }

    {
    auto beg = uut.begin();
    const auto& [firstActualFake1, firstActualFake2] = *beg;
    EXPECT_EQ(0, firstActualFake1->value);
    EXPECT_EQ(3, firstActualFake2->value);
    }

    {
    auto beg = uut.begin();
    const auto& [firstActualFake1, firstActualFake2] = *beg;
    EXPECT_EQ(0, firstActualFake1->value);
    EXPECT_EQ(3, firstActualFake2->value);
    }

        {
    auto beg = uut.begin();
    const auto& [firstActualFake1, firstActualFake2] = *beg;
    EXPECT_EQ(0, firstActualFake1->value);
    EXPECT_EQ(3, firstActualFake2->value);
    }

    auto beg = uut.begin();
    ++beg;
    const auto& [secondActualFake1, secondActualFake2] = *beg;
    EXPECT_EQ(1, secondActualFake1->value);
    EXPECT_EQ(4, secondActualFake2->value);
}

TEST_F(View_unit_tests, InnerJoin_multipleTypes_stlCompatible)
{
    auto ecs = nc::ecs::Ecs{impl};
    auto e1 = ecs.Emplace<Entity>(EntityInfo{});
    auto e2 = ecs.Emplace<Entity>(EntityInfo{});
    auto e3 = ecs.Emplace<Entity>(EntityInfo{});
    ecs.Emplace<Fake1>(e1, 0);
    ecs.Emplace<Fake1>(e2, 1);
    ecs.Emplace<Fake1>(e3, 2);
    ecs.Emplace<Fake2>(e1, 3);
    ecs.Emplace<Fake2>(e2, 4);
    ecs.Emplace<Fake2>(e3, 5);
    ecs.Emplace<Fake3>(e1, 6);
    ecs.Emplace<Fake3>(e2, 7);
    ecs.Emplace<Fake3>(e3, 8);
    impl.CommitPendingChanges();

    std::ranges::for_each(
        nc::ecs::InnerJoin<Fake1, Fake2, Fake3>(ecs),
        [](auto&& tuple){
            const auto& [fake1, fake2, fake3] = tuple;
            EXPECT_EQ(fake1->ParentEntity(), fake2->ParentEntity());
            EXPECT_EQ(fake2->ParentEntity(), fake3->ParentEntity());
        }
    );

    auto toParent = [](auto&& set){
        const auto& [fake1, _1, _2] = set;
        return fake1->ParentEntity();
    };

    auto composedRange =
        nc::ecs::InnerJoin<Fake1, Fake2, Fake3>(ecs) |
        std::views::transform(toParent) |
        std::views::enumerate;

    for (auto [i, entity] : composedRange)
    {
        EXPECT_EQ(i, entity.Index());
    }
}

TEST_F(View_unit_tests, InnerJoinView_sizeUpperBound_returnsSizeOfSmallestPool)
{
    auto ecs = nc::ecs::Ecs{impl};
    auto e1 = ecs.Emplace<Entity>(EntityInfo{});
    auto e2 = ecs.Emplace<Entity>(EntityInfo{});
    auto e3 = ecs.Emplace<Entity>(EntityInfo{});
    ecs.Emplace<Fake1>(e1, 0);
    ecs.Emplace<Fake2>(e1, 3);
    impl.CommitPendingChanges();
    auto actualSize = nc::ecs::InnerJoin<Fake1, Fake2, Fake3>(ecs).size_upper_bound();
    EXPECT_EQ(0, actualSize);

    ecs.Emplace<Fake1>(e2, 1);
    ecs.Emplace<Fake2>(e2, 4);
    ecs.Emplace<Fake3>(e2, 7);
    impl.CommitPendingChanges();
    actualSize = nc::ecs::InnerJoin<Fake1, Fake2, Fake3>(ecs).size_upper_bound();
    EXPECT_EQ(1, actualSize);

    ecs.Emplace<Fake1>(e3, 2);
    ecs.Emplace<Fake2>(e3, 5);
    ecs.Emplace<Fake3>(e3, 8);
    impl.CommitPendingChanges();
    actualSize = nc::ecs::InnerJoin<Fake1, Fake2, Fake3>(ecs).size_upper_bound();
    EXPECT_EQ(2, actualSize);
}


// todo:
// test join_for...
// test view_interface
// test with diff ecs types


TEST_F(View_unit_tests, Single_ForwardIteration_VisitsEach)
{
    auto e1 = registry.Add<Entity>(EntityInfo{});
    auto e2 = registry.Add<Entity>(EntityInfo{});
    auto e3 = registry.Add<Entity>(EntityInfo{});
    registry.Add<Fake1>(e1, 0);
    registry.Add<Fake1>(e2, 1);
    registry.Add<Fake1>(e3, 2);
    registry.CommitStagedChanges();
    auto v = View<Fake1>{&registry};
    EXPECT_EQ(v.size(), 3);
    auto beg = v.begin();
    auto end = v.end();
    auto count = 0;
    for(auto cur = beg; cur != end; ++cur)
    {
        EXPECT_EQ((*cur).value, count);
        EXPECT_EQ(cur->value, count);
        ++count;
    }
}

TEST_F(View_unit_tests, Single_ForwardIteration_ValidAfterAdd)
{
    auto e1 = registry.Add<Entity>(EntityInfo{});
    auto e2 = registry.Add<Entity>(EntityInfo{});
    auto e3 = registry.Add<Entity>(EntityInfo{});
    auto e4 = registry.Add<Entity>(EntityInfo{});
    registry.Add<Fake1>(e1, 0);
    registry.Add<Fake1>(e2, 1);
    registry.CommitStagedChanges();
    auto v = View<Fake1>{&registry};
    registry.Add<Fake1>(e3, 2);
    registry.Add<Fake1>(e4, 3);
    EXPECT_EQ(v.size(), 2);
    auto beg = v.begin();
    auto end = v.end();
    auto count = 0;
    for(auto cur = beg; cur != end; ++cur)
    {
        EXPECT_EQ((*cur).value, count);
        EXPECT_EQ(cur->value, count);
        ++count;
    }
    EXPECT_EQ(count, 2);
}

TEST_F(View_unit_tests, Single_ReverseIteration_VisitsEach)
{
    auto e1 = registry.Add<Entity>(EntityInfo{});
    auto e2 = registry.Add<Entity>(EntityInfo{});
    auto e3 = registry.Add<Entity>(EntityInfo{});
    registry.Add<Fake1>(e1, 0);
    registry.Add<Fake1>(e2, 1);
    registry.Add<Fake1>(e3, 2);
    registry.CommitStagedChanges();
    auto v = View<Fake1>{&registry};
    EXPECT_EQ(v.size(), 3);
    auto rbeg = v.rbegin();
    auto rend = v.rend();
    auto count = 2;
    for(auto cur = rbeg; cur != rend; ++cur)
    {
        EXPECT_EQ((*cur).value, count);
        EXPECT_EQ(cur->value, count);
        --count;
    }
}

TEST_F(View_unit_tests, Single_EmptyView_SkipsLoop)
{
    auto v = View<Fake1>{&registry};
    EXPECT_EQ(v.size(), 0);

    auto beg = v.begin();
    auto end = v.end();
    auto count = 0;
    for(auto cur = beg; cur != end; ++cur)
    {
        ++count;
    }

    EXPECT_EQ(count, 0);

    auto rbeg = v.rbegin();
    auto rend = v.rend();
    for(auto cur = rbeg; cur != rend; ++cur)
    {
        ++count;
    }

    EXPECT_EQ(count, 0);
}

TEST_F(View_unit_tests, Single_ReverseIteration_ValidAfterRemove)
{
    auto e1 = registry.Add<Entity>(EntityInfo{});
    auto e2 = registry.Add<Entity>(EntityInfo{});
    auto e3 = registry.Add<Entity>(EntityInfo{});
    registry.Add<Fake1>(e1, 0);
    registry.Add<Fake1>(e2, 1);
    registry.Add<Fake1>(e3, 2);
    registry.CommitStagedChanges();
    auto v = View<Fake1>{&registry};
    EXPECT_EQ(v.size(), 3);
    auto rbeg = v.rbegin();
    auto rend = v.rend();
    auto count = 2;
    for(auto cur = rbeg; cur != rend; )
    {
        EXPECT_EQ((*cur).value, count);
        EXPECT_EQ(cur->value, count);
        --count;
        auto e = cur->ParentEntity();
        ++cur;
        registry.Remove<Fake1>(e);
    }
}

TEST_F(View_unit_tests, Single_ConstRegistry_ConstructsReadOnlyView)
{
    auto e1 = registry.Add<Entity>(EntityInfo{});
    registry.Add<Fake1>(e1, 0);
    registry.CommitStagedChanges();
    const auto* constRegistry = &registry;
    auto v = View<const Fake1>{constRegistry};
    EXPECT_EQ(v.size(), 1);
}

TEST_F(View_unit_tests, Single_NonConstRegistryConstType_ReturnsReadOnlyValues)
{
    using expected_t = const Fake1&;
    using actual_t = decltype(*View<const Fake1>{&registry}.begin());
    EXPECT_TRUE((std::is_same_v<expected_t, actual_t>));
}

TEST_F(View_unit_tests, Multi_SizeUpperBound)
{
    auto e1 = registry.Add<Entity>(EntityInfo{});
    auto e2 = registry.Add<Entity>(EntityInfo{});
    auto e3 = registry.Add<Entity>(EntityInfo{});
    registry.Add<Fake1>(e1, 0);
    registry.Add<Fake1>(e2, 1);
    registry.Add<Fake1>(e3, 2);
    registry.Add<Fake2>(e2, 1);
    registry.Add<Fake2>(e3, 2);
    registry.Add<Fake3>(e2, 1);
    registry.CommitStagedChanges();

    auto v12 = MultiView<Fake1, Fake2>{&registry};
    EXPECT_EQ(v12.size_upper_bound(), 2);

    auto v23 = MultiView<Fake2, Fake3>{&registry};
    EXPECT_EQ(v23.size_upper_bound(), 1);

    auto v13 = MultiView<Fake1, Fake3>{&registry};
    EXPECT_EQ(v13.size_upper_bound(), 1);

    auto v123 = MultiView<Fake1, Fake2, Fake3>{&registry};
    EXPECT_EQ(v123.size_upper_bound(), 1);
}

TEST_F(View_unit_tests, Multi_VisitsEach)
{
    auto e1 = registry.Add<Entity>(EntityInfo{});
    auto e2 = registry.Add<Entity>(EntityInfo{});
    auto e3 = registry.Add<Entity>(EntityInfo{});
    auto e4 = registry.Add<Entity>(EntityInfo{});
    registry.Add<Fake1>(e1, 1);
    registry.Add<Fake1>(e2, 1);
    registry.Add<Fake1>(e3, 1);
    registry.Add<Fake1>(e4, 1);
    registry.Add<Fake2>(e2, 2);
    registry.Add<Fake2>(e3, 2);
    registry.Add<Fake2>(e4, 2);
    registry.Add<Fake3>(e2, 3);
    registry.Add<Fake3>(e4, 3);
    registry.CommitStagedChanges();

    const auto expectedFake1Value = 10;
    const auto expectedFake2Value = 20;
    const auto expectedFake3Value = 30;

    for(auto& [f1, f2, f3] : MultiView<Fake1, Fake2, Fake3>{&registry})
    {
        f1->value = expectedFake1Value;
        f2->value = expectedFake2Value;
        f3->value = expectedFake3Value;
    }

    EXPECT_EQ(registry.Get<Fake1>(e2)->value, expectedFake1Value);
    EXPECT_EQ(registry.Get<Fake2>(e2)->value, expectedFake2Value);
    EXPECT_EQ(registry.Get<Fake3>(e2)->value, expectedFake3Value);
    EXPECT_EQ(registry.Get<Fake1>(e4)->value, expectedFake1Value);
    EXPECT_EQ(registry.Get<Fake2>(e4)->value, expectedFake2Value);
    EXPECT_EQ(registry.Get<Fake3>(e4)->value, expectedFake3Value);
}

TEST_F(View_unit_tests, Multi_ConstAndNonConstComponents_PropagatesConst)
{
    for(auto& [f1, f2] : MultiView<Fake1, const Fake2>{&registry})
    {
        static_assert(!std::is_const_v<std::remove_pointer_t<decltype(f1)>>);
        static_assert(std::is_const_v<std::remove_pointer_t<decltype(f2)>>);
    }
}

int main(int argc, char ** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
