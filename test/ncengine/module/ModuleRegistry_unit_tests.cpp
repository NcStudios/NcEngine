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

class ModuleRegistryTests : public ::testing::Test
{
    public:
        ModuleRegistry uut = ModuleRegistry{};

        ~ModuleRegistryTests() noexcept
        {
            // clear any static pts from previous tests
            uut.Unregister<TestModule1>();
            uut.Unregister<TestModule2>();
        }
};

TEST_F(ModuleRegistryTests, Register_NewModule_RegistersModule)
{
    auto module1 = std::make_unique<TestModule1>();
    uut.Register<TestModule1>(std::move(module1));

    EXPECT_TRUE(uut.IsRegistered<TestModule1>());
}

TEST_F(ModuleRegistryTests, Register_ExistingModule_ReplacesModule)
{
    auto module1 = std::make_unique<TestModule1>();
    auto module2 = std::make_unique<TestModule1>();
    const auto* expected = module2.get();

    uut.Register<TestModule1>(std::make_unique<TestModule1>());
    uut.Register<TestModule1>(std::move(module2));

    EXPECT_TRUE(uut.IsRegistered<TestModule1>());
    EXPECT_EQ(uut.Get<TestModule1>(), expected);
    EXPECT_EQ(uut.GetAllModules().size(), 1);
}

TEST_F(ModuleRegistryTests, Register_HasExplicitId_DoesNotModifyId)
{
    uut.Register<TestModule1>(std::make_unique<TestModule1>());
    const auto actual = uut.Get<TestModule1>();
    EXPECT_EQ(TestModule1::expectedId, actual->Id());
}

TEST_F(ModuleRegistryTests, Register_HasDefaultId_AssignsAvailableid)
{
    uut.Register<TestModule2>(std::make_unique<TestModule2>());
    const auto actual = uut.Get<TestModule2>();
    EXPECT_EQ(std::numeric_limits<size_t>::max(), actual->Id());
}

TEST_F(ModuleRegistryTests, Register_IdAlreadyInUse_Throws)
{
    uut.Register<TestModule1>(std::make_unique<TestModule1>());
    EXPECT_THROW(uut.Register<TestModuleBadId>(std::make_unique<TestModuleBadId>()), NcError);
}

TEST_F(ModuleRegistryTests, Unregister_ExistingModule_Unregisters)
{
    uut.Register<TestModule1>(std::make_unique<TestModule1>());
    uut.Unregister<TestModule1>();

    EXPECT_FALSE(uut.IsRegistered<TestModule1>());
    EXPECT_EQ(uut.Get<TestModule1>(), nullptr);
    EXPECT_EQ(uut.GetAllModules().size(), 0);
}

TEST_F(ModuleRegistryTests, Unregister_NonexistentModule_Succeeds)
{
    uut.Unregister<TestModule1>();

    EXPECT_FALSE(uut.IsRegistered<TestModule1>());
    EXPECT_EQ(uut.Get<TestModule1>(), nullptr);
    EXPECT_EQ(uut.GetAllModules().size(), 0);
}

TEST_F(ModuleRegistryTests, IsRegistered_ModuleRegistered_ReturnsTrue)
{
    uut.Register<TestModule1>(std::make_unique<TestModule1>());
    EXPECT_TRUE(uut.IsRegistered<TestModule1>());
    EXPECT_TRUE(uut.IsRegistered(TestModule1::expectedId));
}

TEST_F(ModuleRegistryTests, IsRegistered_ModuleNotRegistered_ReturnsFalse)
{
    uut.Register<TestModule2>(std::make_unique<TestModule2>());
    EXPECT_FALSE(uut.IsRegistered<TestModule1>());
    EXPECT_FALSE(uut.IsRegistered(TestModule1::expectedId));
}

TEST_F(ModuleRegistryTests, Get_ModuleRegistered_ReturnsPointerToModule)
{
    auto module = std::make_unique<TestModule1>();
    const auto expectedPtr = module.get();
    uut.Register<TestModule1>(std::move(module));
    const auto actualPtrByType = uut.Get<TestModule1>();
    const auto actualPtrById = uut.Get(TestModule1::expectedId);
    EXPECT_EQ(expectedPtr, actualPtrByType);
    EXPECT_EQ(expectedPtr, actualPtrById);
}

TEST_F(ModuleRegistryTests, Get_ModuleNotRegistered_ReturnsNull)
{
    const auto actualPtrByType = uut.Get<TestModule1>();
    const auto actualPtrById = uut.Get(TestModule1::expectedId);
    EXPECT_EQ(nullptr, actualPtrByType);
    EXPECT_EQ(nullptr, actualPtrById);
}
