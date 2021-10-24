#include "gtest/gtest.h"
#include "component/Transform.h"
#include "types/EngineTypes.h"
#include "ECS.h"


using namespace nc;


class Types_unit_tests : public ::testing::Test
{
    public:
        std::unique_ptr<registry_type> registry;

    protected:
        void SetUp() override
        {
            registry = std::make_unique<registry_type>(100u);
            internal::SetActiveRegistry(registry.get());
        }

        void TearDown() override
        {
            registry->Clear();
            internal::SetActiveRegistry(nullptr);
        }
};

TEST_F(Types_unit_tests, Types_HaveName)
{
    EXPECT_EQ((std::string)type::Int->Name(), "int");
}

TEST_F(Types_unit_tests, Types_ByName)
{
    EXPECT_EQ(Types::Get("int"), type::Int);
}

TEST_F(Types_unit_tests, Types_Access_Embedded_Get)
{
    auto e = registry->Add<Entity>(EntityInfo{.position = Vector3(1, 2, 3)});
    auto accessPosition = type::Entity->GetAccess<Vector3>({"transform", "position"});

    // Has access
    EXPECT_EQ((bool)accessPosition, true);

    auto getPosition = accessPosition(e);

    // Can get position
    EXPECT_EQ((bool)getPosition, true);
    EXPECT_EQ(getPosition->x, 1);
    EXPECT_EQ(getPosition->y, 2);
    EXPECT_EQ(getPosition->z, 3);
}

TEST_F(Types_unit_tests, Types_Access_Embedded_Set)
{
    auto e = registry->Add<Entity>(EntityInfo{.position = Vector3(1, 2, 3)});
    auto accessPosition = type::Entity->GetAccess<Vector3>({"transform", "position"});

    // Has access
    EXPECT_EQ((bool)accessPosition, true);

    // Set position
    accessPosition(e, Vector3(4, 5, 6));

    auto getPosition = accessPosition(e);
    EXPECT_EQ(getPosition->x, 4);
    EXPECT_EQ(getPosition->y, 5);
    EXPECT_EQ(getPosition->z, 6);
}

TEST_F(Types_unit_tests, Types_Access_Get)
{
    auto v = Vector3(3, 4, 5);
    auto accessX = type::Vector3->GetAccess<float>({"x"});
    auto x = accessX(v); // get

    EXPECT_EQ((bool)x, true);
    EXPECT_EQ(x.get(), 3);
}

int main(int argc, char ** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}