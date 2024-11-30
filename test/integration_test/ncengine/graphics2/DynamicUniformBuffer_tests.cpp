#include "DiligentEngineFixture.inl"
#include "graphics2/diligent/resource/base/DynamicUniformBuffer.h"
#include "ncmath/Vector.h"

struct TestObject
{
    nc::Vector3 v1 = nc::Vector3::Zero();
    float v2 = 0.0f;
};

struct LargeTestObject
{
    nc::Vector3 v1 = nc::Vector3::Zero();
    nc::Vector3 v2 = nc::Vector3::Zero();
};

class DynamicUniformBufferTest : public DiligentEngineFixture
{
    protected:
        nc::graphics::DynamicUniformBuffer uut;

        DynamicUniformBufferTest()
            : uut{
                engine->GetContext(),
                engine->GetDevice(),
                TestObject{},
                "TestBuffer"
            }
        {
        }

        ~DynamicUniformBufferTest()
        {
            FailIfHasErrorOutput();
        }
};

TEST_F(DynamicUniformBufferTest, WriteCases)
{
    // Has expected size
    constexpr auto expectedSize = sizeof(TestObject);
    EXPECT_EQ(expectedSize, uut.GetSize());
    EXPECT_EQ(expectedSize, uut.GetBuffer().GetDesc().Size);

    // Write succeeds
    // note: we don't have a way to verify contents as dynamic usage buffers cannot be created with read access
    const auto overwrite = TestObject{nc::Vector3::One(), 42.0f};
    EXPECT_NO_THROW(uut.Write(engine->GetContext(), overwrite));

    // Map returns valid region
    auto mapped = uut.Map(engine->GetContext());
    EXPECT_NE(nullptr, mapped);
    uut.Unmap(engine->GetContext());

    // Unexpected size throws
    EXPECT_THROW(uut.Write(engine->GetContext(), float{}), nc::NcError);
    EXPECT_THROW(uut.Write(engine->GetContext(), LargeTestObject{}), nc::NcError);
}
