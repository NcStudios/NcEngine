#include "DiligentEngineFixture.inl"
#include "graphics2/diligent/resource/ResourceTypes.h"
#include "graphics2/diligent/resource/base/StructuredBuffer.h"

#include "ncutility/NcError.h"

#include <array>

class StructuredBufferTest : public DiligentEngineFixture
{
    protected:
        static constexpr auto signatureName = "testSignature";
        static constexpr auto variableName = "testBuffer";
        static constexpr auto bindingIndex = uint8_t{1};
        static constexpr auto maxInstanceCount = 10u;
        static constexpr auto initialInstanceCount = 3u;

        Diligent::RefCntAutoPtr<Diligent::IPipelineResourceSignature> signature;
        Diligent::RefCntAutoPtr<Diligent::IShaderResourceBinding> srb;
        std::unique_ptr<nc::graphics::StructuredBuffer<nc::graphics::MaterialData>> uut = nullptr;
        std::array<nc::graphics::MaterialData, initialInstanceCount> properties;

        StructuredBufferTest()
        {
            const auto bufferDesc = nc::graphics::StructuredBufferDesc{
                variableName,
                Diligent::SHADER_TYPE::SHADER_TYPE_PIXEL,
                maxInstanceCount,
                initialInstanceCount
            };

            const auto resources = std::array{
                ToPipelineResourceDesc(bufferDesc)
            };

            auto desc = Diligent::PipelineResourceSignatureDesc{};
            desc.Name = signatureName;
            desc.Resources = resources.data(),
            desc.NumResources = static_cast<uint32_t>(resources.size()),
            desc.BindingIndex = bindingIndex,
            engine->GetDevice().CreatePipelineResourceSignature(desc, &signature);

            if (!signature)
            {
                throw nc::NcError{"Failed to create resource signature"};
            }

            signature->CreateShaderResourceBinding(&srb);
            if (!srb)
            {
                throw nc::NcError{"Failed to create shader resource binding"};
            }

            auto var = srb->GetVariableByName(bufferDesc.shaderType, variableName);


            uut = std::make_unique<nc::graphics::StructuredBuffer<nc::graphics::MaterialData>>
            (
                engine->GetContext(),
                engine->GetDevice(),
                *var,
                bufferDesc
            );
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
    context.CommitShaderResources(srb, Diligent::RESOURCE_STATE_TRANSITION_MODE_VERIFY);

    // update with no dirty elements succeeds
    auto updateInfo = nc::graphics::BufferUpdateInfo<nc::graphics::MaterialData>{ .instances = properties, .dirtyRanges = {} };
    uut->Update(context, device, updateInfo);
    context.CommitShaderResources(srb, Diligent::RESOURCE_STATE_TRANSITION_MODE_VERIFY);

    // update entire range succeeds
    updateInfo.dirtyRanges = { {0, initialInstanceCount} };
    uut->Update(context, device, updateInfo);
    context.CommitShaderResources(srb, Diligent::RESOURCE_STATE_TRANSITION_MODE_VERIFY);

    // update individual materials succeeds
    updateInfo.dirtyRanges = { {0, 1}, {1, 2} };
    uut->Update(context, device, updateInfo);
    context.CommitShaderResources(srb, Diligent::RESOURCE_STATE_TRANSITION_MODE_VERIFY);

    // update requires buffer recreation succeeds
    std::array<nc::graphics::MaterialData, initialInstanceCount * 2> largerProperties;
    updateInfo.instances = largerProperties;
    updateInfo.dirtyRanges = { {initialInstanceCount, initialInstanceCount * 2} };
    uut->Update(context, device, updateInfo);
    context.CommitShaderResources(srb, Diligent::RESOURCE_STATE_TRANSITION_MODE_VERIFY);
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
    context.CommitShaderResources(srb, Diligent::RESOURCE_STATE_TRANSITION_MODE_VERIFY);
}

TEST_F(StructuredBufferTest, AllocateBufferCases)
{
    auto& context = engine->GetContext();
    auto& device = engine->GetDevice();

    // Reallocate with no initial data succeeds
    EXPECT_NO_THROW(uut->CreateBuffer(context, device, 2));
    EXPECT_EQ(2, uut->GetElementCount());
    context.CommitShaderResources(srb, Diligent::RESOURCE_STATE_TRANSITION_MODE_VERIFY);

    // Reallocate with initial data succeeds
    auto data = std::array<nc::graphics::MaterialData, 3>{};
    EXPECT_NO_THROW(uut->CreateBuffer(context, device, data));
    context.CommitShaderResources(srb, Diligent::RESOURCE_STATE_TRANSITION_MODE_VERIFY);

    // Invalid buffer size fails
    EXPECT_THROW(uut->CreateBuffer(context, device, 0), nc::NcError);
    EXPECT_THROW(uut->CreateBuffer(context, device, maxInstanceCount + 1), nc::NcError);
    EXPECT_THROW(uut->CreateBuffer(context, device, std::span<nc::graphics::MaterialData>{}), nc::NcError);
    EXPECT_THROW(uut->CreateBuffer(context, device, std::array<nc::graphics::MaterialData, maxInstanceCount + 1>{}), nc::NcError);
}
