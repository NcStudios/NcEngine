#include "gtest/gtest.h"
#include "ecs/View.h"

using namespace nc;

namespace nc
{
Transform::Transform(Entity entity, const Vector3&, const Quaternion&, const Vector3&, Entity parent)
    : ComponentBase{entity}, m_localMatrix{}, m_worldMatrix{}, m_parent{parent}, m_children{}
{}

void Transform::SetParent(Entity entity) { m_parent = entity; }

std::span<Entity> Transform::Children() { return std::span<Entity>{m_children}; }

Quaternion::Quaternion(float X, float Y, float Z, float W)
    : x{X}, y{Y}, z{Z}, w{W}
{}

namespace ecs::detail
{
FreeComponentGroup::FreeComponentGroup(Entity entity) : ComponentBase{entity} {}
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


Registry g_registry{10u};

class View_unit_tests : public ::testing::Test
{
    public:
        Registry& registry;

        View_unit_tests()
            : registry{g_registry}
        {
            registry.RegisterComponentType<Fake1>();
            registry.RegisterComponentType<Fake2>();
            registry.RegisterComponentType<Fake3>();
        }

        ~View_unit_tests()
        {
            registry.CommitStagedChanges();
            registry.Clear();
        }
};

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