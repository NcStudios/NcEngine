#include "DiligentEngineFixture.inl"
#include "graphics2/diligent/resource/component/MeshRendererBufferResource.h"
#include "graphics2/diligent/resource/component/ComponentResourceSignature.h"
#include "graphics2/frontend/subsystem/MeshRendererRenderState.h"
#include "ncengine/graphics/Material.h"

#include <array>

class MeshRendererBufferResource : public DiligentEngineFixture
{
    protected:
        static constexpr auto signatureName = "testSignature";
        static constexpr auto variableName = "testBuffer";
        static constexpr auto bindingIndex = uint8_t{1};
        static constexpr auto initialInstanceHint = 3u;

        std::unique_ptr<nc::graphics::ComponentResourceSignature> signature;
        nc::graphics::MeshRendererBufferResource* uut = nullptr;
        nc::graphics::MeshRendererRenderState properties;

        void SetUp() override
        {
            signature = std::make_unique<nc::graphics::ComponentResourceSignature>(
                engine->GetContext(),
                engine->GetDevice(),
                signatureName,
                variableName,
                bindingIndex,
                initialInstanceHint
            );

            uut = &signature->GetMeshRendererBuffer();
        }

        void TearDown() override
        {
            FailIfHasErrorOutput();
        }
};

TEST_F(MeshRendererBufferResource, UpdateCases_succeed)
{
    auto& context = engine->GetContext();

    // commit succeeds with initial/empty buffer
    signature->Commit(context);

    // update with one element succeeds
    properties.modelMatrices = std::vector<DirectX::XMMATRIX>{DirectX::XMMatrixIdentity()};
    properties.entities = std::vector<nc::Entity>{nc::Entity{}};

    uut->Update(properties, context);
    signature->Commit(context);
}
