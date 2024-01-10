#include "gtest/gtest.h"
#include "module/ModuleRegistry.h"

using namespace nc;

struct TestModule1 : public Module
{
    static constexpr size_t expectedId = 1ull;

    TestModule1() : Module{expectedId} {}
    void Clear() noexcept override { cleared = true; }
    bool cleared = false;
};

struct TestModule2 : public Module
{
    void Clear() noexcept override { cleared = true; }
    bool cleared = false;
};

struct TestModuleBadId : public Module
{
    TestModuleBadId() : Module{TestModule1::expectedId} {}
};

TEST(ModuleRegistry_unit_tests, Register_NewModule_RegistersModule)
{
    auto uut = ModuleRegistry{};
    auto module1 = std::make_unique<TestModule1>();
    uut.Register<TestModule1>(std::move(module1));

    EXPECT_TRUE(uut.IsRegistered<TestModule1>());
}

TEST(ModuleRegistry_unit_tests, Register_ExistingModule_ReplacesModule)
{
    auto module1 = std::make_unique<TestModule1>();
    auto module2 = std::make_unique<TestModule1>();
    const auto* expected = module2.get();

    auto uut = ModuleRegistry{};
    uut.Register<TestModule1>(std::make_unique<TestModule1>());
    uut.Register<TestModule1>(std::move(module2));

    EXPECT_TRUE(uut.IsRegistered<TestModule1>());
    EXPECT_EQ(uut.Get<TestModule1>(), expected);
    EXPECT_EQ(uut.GetAllModules().size(), 1);
}

TEST(ModuleRegistry_unit_tests, Register_HasExplicitId_DoesNotModifyId)
{
    auto uut = ModuleRegistry{};
    uut.Register<TestModule1>(std::make_unique<TestModule1>());
    const auto actual = uut.Get<TestModule1>();
    EXPECT_EQ(TestModule1::expectedId, actual->Id());
}

TEST(ModuleRegistry_unit_tests, Register_HasDefaultId_AssignsAvailableid)
{
    auto uut = ModuleRegistry{};
    uut.Register<TestModule2>(std::make_unique<TestModule2>());
    const auto actual = uut.Get<TestModule2>();
    EXPECT_EQ(std::numeric_limits<size_t>::max(), actual->Id());
}

TEST(ModuleRegistry_unit_tests, Register_IdAlreadyInUse_Throws)
{
    auto uut = ModuleRegistry{};
    uut.Register<TestModule1>(std::make_unique<TestModule1>());
    EXPECT_THROW(uut.Register<TestModuleBadId>(std::make_unique<TestModuleBadId>()), NcError);
}

TEST(ModuleRegistry_unit_tests, Unregister_ExistingModule_Unregisters)
{
    auto uut = ModuleRegistry{};
    uut.Register<TestModule1>(std::make_unique<TestModule1>());
    uut.Unregister<TestModule1>();

    EXPECT_FALSE(uut.IsRegistered<TestModule1>());
    EXPECT_EQ(uut.Get<TestModule1>(), nullptr);
    EXPECT_EQ(uut.GetAllModules().size(), 0);
}

TEST(ModuleRegistry_unit_tests, Unregister_NonexistentModule_Succeeds)
{
    auto uut = ModuleRegistry{};
    uut.Unregister<TestModule1>();

    EXPECT_FALSE(uut.IsRegistered<TestModule1>());
    EXPECT_EQ(uut.Get<TestModule1>(), nullptr);
    EXPECT_EQ(uut.GetAllModules().size(), 0);
}

TEST(ModuleRegistry_unit_tests, IsRegistered_ModuleRegistered_ReturnsTrue)
{
    auto uut = ModuleRegistry{};

    // clear any static pts from previous tests
    uut.Unregister<TestModule1>();
    uut.Unregister<TestModule2>();

    uut.Register<TestModule1>(std::make_unique<TestModule1>());
    EXPECT_TRUE(uut.IsRegistered<TestModule1>());
    EXPECT_TRUE(uut.IsRegistered(TestModule1::expectedId));
}

TEST(ModuleRegistry_unit_tests, IsRegistered_ModuleNotRegistered_ReturnsFalse)
{
    auto uut = ModuleRegistry{};

    // clear any static pts from previous tests
    uut.Unregister<TestModule1>();
    uut.Unregister<TestModule2>();

    uut.Register<TestModule2>(std::make_unique<TestModule2>());
    EXPECT_FALSE(uut.IsRegistered<TestModule1>());
    EXPECT_FALSE(uut.IsRegistered(TestModule1::expectedId));
}

int main(int argc, char ** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
