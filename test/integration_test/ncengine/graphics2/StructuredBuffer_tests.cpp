#include "DiligentEngineFixture.inl"
#include "graphics2/diligent/resource/base/StructuredBuffer.h"
#include "graphics2/diligent/resource/MaterialResourceSignature.h"
#include "graphics2/frontend/subsystem/MaterialRegistry.h"
#include "ncengine/graphics/Material.h"

#include <array>

class StructuredBufferTest : public DiligentEngineFixture
{
    protected:
        static constexpr auto signatureName = "testSignature";
        static constexpr auto variableName = "testBuffer";
        static constexpr auto bindingIndex = uint8_t{1};
        static constexpr auto maxInstanceCount = 10u;
        static constexpr auto initialInstanceCount = 3u;

        std::unique_ptr<nc::graphics::MaterialResourceSignature> signature;
        nc::graphics::StructuredBuffer<nc::graphics::MaterialData>* uut = nullptr;
        std::array<nc::graphics::MaterialData, initialInstanceCount> properties;

        StructuredBufferTest()
        {
            signature = std::make_unique<nc::graphics::MaterialResourceSignature>(
                engine->GetContext(),
                engine->GetDevice(),
                signatureName,
                variableName,
                bindingIndex,
                maxInstanceCount,
                initialInstanceCount
            );

            uut = &signature->GetMaterialDataResource();
        }

        ~StructuredBufferTest()
        {
            FailIfHasErrorOutput();
        }
};

TEST_F(StructuredBufferTest, UpdateCases_succeed)
{
    auto& context = engine->GetContext();
    auto& device = engine->GetDevice();

    // commit succeeds with initial/empty buffer
    signature->Commit(context);

    // update with no dirty elements succeeds
    auto updateInfo = nc::graphics::BufferUpdateInfo<nc::graphics::MaterialData>{ .instances = properties, .dirtyRanges = {} };
    uut->Update(context, device, updateInfo);
    signature->Commit(context);

    // update entire range succeeds
    updateInfo.dirtyRanges = { {0, initialInstanceCount} };
    uut->Update(context, device, updateInfo);
    signature->Commit(context);

    // update individual materials succeeds
    updateInfo.dirtyRanges = { {0, 1}, {1, 2} };
    uut->Update(context, device, updateInfo);
    signature->Commit(context);

    // update requires buffer recreation succeeds
    std::array<nc::graphics::MaterialData, initialInstanceCount * 2> largerProperties;
    updateInfo.instances = largerProperties;
    updateInfo.dirtyRanges = { {initialInstanceCount, initialInstanceCount * 2} };
    uut->Update(context, device, updateInfo);
    signature->Commit(context);
}

TEST_F(StructuredBufferTest, WriteCases)
{
    auto& context = engine->GetContext();
    auto data = std::vector<nc::graphics::MaterialData>(maxInstanceCount + 1);
    auto wholeRange = std::span{data.begin(), data.end()};
    auto allocatedRange = std::span{data.begin(), data.begin() + 3};
    auto partialRange = std::span{data.begin() + 1, data.begin() + 2};

    // Verify uut + test data size expectations
    ASSERT_EQ(maxInstanceCount, uut->GetMaxElementCount());
    ASSERT_EQ(initialInstanceCount, uut->GetElementCount());
    ASSERT_EQ(initialInstanceCount * uut->ElementStride, uut->SizeBytes());
    ASSERT_GT(wholeRange.size(), uut->GetElementCount());
    ASSERT_EQ(allocatedRange.size(), uut->GetElementCount());
    ASSERT_LT(partialRange.size(), uut->GetElementCount());

    // Valid writes succeed
    uut->Transition(context, Diligent::RESOURCE_STATE_COPY_DEST);
    EXPECT_NO_THROW(uut->Write(context, allocatedRange, 0));
    EXPECT_NO_THROW(uut->Write(context, partialRange, 0));
    EXPECT_NO_THROW(uut->Write(context, partialRange, 1));

    // Expect no changes to size
    EXPECT_EQ(initialInstanceCount, uut->GetElementCount());
    EXPECT_EQ(initialInstanceCount * uut->ElementStride, uut->SizeBytes());

    // Out of bounds due to large source data fails
    EXPECT_THROW(uut->Write(context, wholeRange, 0), nc::NcError);

    // Out of bounds due to offset fails
    EXPECT_THROW(uut->Write(context, allocatedRange, 1), nc::NcError);

    uut->Transition(context, Diligent::RESOURCE_STATE_SHADER_RESOURCE);
    signature->Commit(context);
}

TEST_F(StructuredBufferTest, AllocateBufferCases)
{
    auto& context = engine->GetContext();
    auto& device = engine->GetDevice();

    // Reallocate with no initial data succeeds
    EXPECT_NO_THROW(uut->CreateBuffer(context, device, 2));
    EXPECT_EQ(2, uut->GetElementCount());
    signature->Commit(context);

    // Reallocate with initial data succeeds
    auto data = std::array<nc::graphics::MaterialData, 3>{};
    EXPECT_NO_THROW(uut->CreateBuffer(context, device, data));
    signature->Commit(context);

    // Invalid buffer size fails
    EXPECT_THROW(uut->CreateBuffer(context, device, 0), nc::NcError);
    EXPECT_THROW(uut->CreateBuffer(context, device, maxInstanceCount + 1), nc::NcError);
    EXPECT_THROW(uut->CreateBuffer(context, device, std::span<nc::graphics::MaterialData>{}), nc::NcError);
    EXPECT_THROW(uut->CreateBuffer(context, device, std::array<nc::graphics::MaterialData, maxInstanceCount + 1>{}), nc::NcError);
}
