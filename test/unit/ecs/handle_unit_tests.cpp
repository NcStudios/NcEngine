#include "gtest/gtest.h"
#include "ecs/handle.h"

using namespace nc;

constexpr auto Entity1 = Entity{0, 0, 0};
constexpr auto Entity2 = Entity{1, 0, 0};

struct Fake : public ComponentBase
{
    Fake(Entity ent) : ComponentBase{ent} {}
};

struct RegistryMock
{
    template<class T>
    auto Add(Entity) -> T* { return nullptr; }

    template<class T>
    void Remove(Entity) { }

    template<class T>
    bool Contains(Entity) { return false; }

    template<class T>
    auto Get(Entity) -> T* { return nullptr; }
};

TEST(handle_unit_tests, Construction)
{
    RegistryMock registry;
    auto h = handle_base<RegistryMock>{&registry, Entity1};
    EXPECT_EQ(h.entity(), Entity1);
    EXPECT_EQ(h.registry(), &registry);
    EXPECT_TRUE(h.valid());
}

TEST(handle_unit_tests, DelegatesToRegistry)
{
    RegistryMock registry;
    auto h = handle_base<RegistryMock>{&registry, Entity1};
    EXPECT_EQ(h.add<Fake>(), nullptr);
    EXPECT_FALSE(h.contains<Fake>());
    EXPECT_EQ(h.get<Fake>(), nullptr);
}

TEST(handle_unit_tests, Equality)
{
    RegistryMock registry;
    auto original = handle_base<RegistryMock>{&registry, Entity1};
    auto same = handle_base<RegistryMock>{&registry, Entity1};
    auto different = handle_base<RegistryMock>{&registry, Entity2};
    EXPECT_EQ(original, same);
    EXPECT_NE(original, different);
}

int main(int argc, char ** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}