#include "gtest/gtest.h"
#include "module/ModuleRegistry.h"

using namespace nc;

struct TestModule1 : public Module
{
    void Clear() noexcept override { cleared = true; }
    bool cleared = false;
};

struct TestModule2 : public Module
{
    void Clear() noexcept override { cleared = true; }
    bool cleared = false;
};

TEST(ModuleRegistry_tests, Register_NewModule_RegistersModule)
{
    auto uut = ModuleRegistry{};
    auto module1 = std::make_unique<TestModule1>();
    uut.Register<TestModule1>(std::move(module1));

    EXPECT_TRUE(uut.IsRegistered<TestModule1>());
}

TEST(ModuleRegistry_tests, Register_ExistingModule_ReplacesModule)
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

TEST(ModuleRegistry_tests, Unregister_ExistingModule_Unregisters)
{
    auto uut = ModuleRegistry{};
    uut.Register<TestModule1>(std::make_unique<TestModule1>());
    uut.Unregister<TestModule1>();

    EXPECT_FALSE(uut.IsRegistered<TestModule1>());
    EXPECT_EQ(uut.Get<TestModule1>(), nullptr);
    EXPECT_EQ(uut.GetAllModules().size(), 0);
}

TEST(ModuleRegistry_tests, Unregister_NonexistentModule_Succeeds)
{
    auto uut = ModuleRegistry{};
    uut.Unregister<TestModule1>();

    EXPECT_FALSE(uut.IsRegistered<TestModule1>());
    EXPECT_EQ(uut.Get<TestModule1>(), nullptr);
    EXPECT_EQ(uut.GetAllModules().size(), 0);
}

TEST(ModuleRegistry_tests, Clear_NotifiesModules)
{
    auto uut = ModuleRegistry{};
    uut.Register<TestModule1>(std::make_unique<TestModule1>());
    uut.Register<TestModule2>(std::make_unique<TestModule2>());
    uut.Clear();
    EXPECT_TRUE(uut.Get<TestModule1>()->cleared);
    EXPECT_TRUE(uut.Get<TestModule2>()->cleared);
}

int main(int argc, char ** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}