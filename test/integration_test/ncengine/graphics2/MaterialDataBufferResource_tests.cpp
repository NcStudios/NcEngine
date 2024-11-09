#include "DiligentEngineParameterizedFixture.inl"
#include "graphics2/diligent/resource/MaterialDataBufferResource.h"
#include "graphics2/diligent/resource/MaterialResourceSignature.h"
#include "graphics2/frontend/subsystem/MaterialRegistry.h"
#include "ncengine/graphics/Material.h"

#include <array>

class MaterialDataBufferResourceTest : public DiligentEngineParameterizedFixture
{
    protected:
        static constexpr auto signatureName = "testSignature";
        static constexpr auto variableName = "testBuffer";
        static constexpr auto bindingIndex = uint8_t{1};
        static constexpr auto initialInstanceHint = 3u;

        std::unique_ptr<nc::graphics::MaterialResourceSignature> signature;
        nc::graphics::MaterialDataBufferResource* uut = nullptr;
        std::array<nc::graphics::MaterialData, initialInstanceHint> properties;

        void SetUp() override
        {
            INITIALIZE_DILIGENT_FIXTURE;
            signature = std::make_unique<nc::graphics::MaterialResourceSignature>(
                engine->GetContext(),
                engine->GetDevice(),
                signatureName,
                variableName,
                bindingIndex,
                initialInstanceHint
            );

            uut = &signature->GetMaterialDataResource();
        }

        void TearDown() override
        {
            FailIfHasErrorOutput();
        }
};

INSTANTIATE_TEST_SUITE_P(AllApis, MaterialDataBufferResourceTest, g_apiParams);

TEST_P(MaterialDataBufferResourceTest, UpdateCases_succeed)
{
    auto& context = engine->GetContext();
    auto& device = engine->GetDevice();

    // commit succeeds with initial/empty buffer
    signature->Commit(context);

    // update with no dirty elements succeeds
    auto updateInfo = nc::graphics::MaterialDataUpdateInfo{ .instances = properties, .dirtyRanges = {} };
    uut->Update(updateInfo, context, device);
    signature->Commit(context);

    // update entire range succeeds
    updateInfo.dirtyRanges = { {0, initialInstanceHint} };
    uut->Update(updateInfo, context, device);
    signature->Commit(context);

    // update individual materials succeeds
    updateInfo.dirtyRanges = { {0, 1}, {1, 2} };
    uut->Update(updateInfo, context, device);
    signature->Commit(context);

    // update requires buffer recreation succeeds
    std::array<nc::graphics::MaterialData, initialInstanceHint * 2> largerProperties;
    updateInfo.instances = largerProperties;
    updateInfo.dirtyRanges = { {initialInstanceHint, initialInstanceHint * 2} };
    uut->Update(updateInfo, context, device);
    signature->Commit(context);
}
