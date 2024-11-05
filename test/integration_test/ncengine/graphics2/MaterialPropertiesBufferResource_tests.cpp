#include "DiligentEngineParameterizedFixture.inl"
#include "graphics2/diligent/resource/MaterialPropertiesBufferResource.h"
#include "graphics2/diligent/resource/MaterialResourceSignature.h"
#include "graphics2/frontend/subsystem/MaterialRegistry.h"
#include "ncengine/graphics/Material.h"

#include <array>

class MaterialPropertiesBufferResourceTest : public DiligentEngineParameterizedFixture
{
    protected:
        static constexpr auto signatureName = "testSignature";
        static constexpr auto variableName = "testBuffer";
        static constexpr auto bindingIndex = uint8_t{1};
        static constexpr auto initialInstanceHint = 3u;

        std::unique_ptr<nc::graphics::MaterialResourceSignature> uut;
        std::array<nc::graphics::MaterialProperties, initialInstanceHint> properties;

        MaterialPropertiesBufferResourceTest()
            : DiligentEngineParameterizedFixture{false}
        {
        }

        void SetUp() override
        {
            INITIALIZE_DILIGENT_FIXTURE;
            uut = std::make_unique<nc::graphics::MaterialResourceSignature>(
                engine->GetContext(),
                engine->GetDevice(),
                signatureName,
                variableName,
                bindingIndex,
                initialInstanceHint
            );
        }

        void TearDown() override
        {
            FailIfHasErrorOutput();
        }

};

INSTANTIATE_TEST_SUITE_P(AllApis, MaterialPropertiesBufferResourceTest, g_apiParams);

TEST_P(MaterialPropertiesBufferResourceTest, Foo)
{
    auto& context = engine->GetContext();
    auto& device = engine->GetDevice();
    auto& resource = uut->GetMaterialPropertiesResource();

    // commit succeeds with initial/empty buffer
    uut->Commit(context);

    // update with no dirty elements succeeds
    auto updateInfo = nc::graphics::MaterialPropertyUpdateInfo{ .instances = properties, .dirtyRanges = {} };
    resource.Update(updateInfo, context, device);
    uut->Commit(context);

    // update entire range succeeds
    updateInfo.dirtyRanges = { {0, initialInstanceHint} };
    resource.Update(updateInfo, context, device);
    uut->Commit(context);

    // update individual materials succeeds
    updateInfo.dirtyRanges = { {0, 1}, {1, 2} };
    resource.Update(updateInfo, context, device);
    uut->Commit(context);

    // update requires buffer recreation succeeds
    std::array<nc::graphics::MaterialProperties, initialInstanceHint * 2> largerProperties;
    updateInfo.instances = largerProperties;
    updateInfo.dirtyRanges = { {initialInstanceHint, initialInstanceHint * 2} };
    resource.Update(updateInfo, context, device);
    uut->Commit(context);
}
