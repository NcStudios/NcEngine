#include "gtest/gtest.h"
#include "alloc/Utility.h"

unsigned AllocCount = 0u;
unsigned DeleteCount = 0u;

void TestInit()
{
    AllocCount = 0u;
    DeleteCount = 0u;
}

template<class T>
class TestAlloc
{
    public:
        using value_type = T;

        static T* lastAllocation;

        T* allocate(size_t)
        {
            ++AllocCount;
            lastAllocation = new T;
            return lastAllocation;
        }

        void deallocate(T* ptr, size_t) noexcept
        {
            ++DeleteCount;
            delete ptr;
        }
};

template<class T>
T* TestAlloc<T>::lastAllocation = nullptr;

TEST(MakeUnique_unit_tests, ReturnValue_UnderlyingPtr_InitializedFromAllocator)
{
    TestInit();
    auto ptr = nc::alloc::make_unique<int, TestAlloc<int>>();
    EXPECT_EQ(AllocCount, 1u);
    EXPECT_EQ(TestAlloc<int>::lastAllocation, ptr.get());
}

TEST(MakeUnique_unit_tests, ReturnValue_OnDestruction_CallsDeleter)
{
    TestInit();

    {
        auto ptr = nc::alloc::make_unique<int, TestAlloc<int>>();
    }

    EXPECT_EQ(DeleteCount, 1u);
}

TEST(MakeUnique_unit_tests, ReturnValue_WithBasicDeleter_HasNoSizeOverhead)
{
    TestInit();

    auto ptr = nc::alloc::make_unique<int, TestAlloc<int>>();
    EXPECT_EQ(sizeof(ptr), sizeof(void*));
}

int main(int argc, char ** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}