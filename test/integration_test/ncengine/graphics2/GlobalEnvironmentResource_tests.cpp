#include "DiligentEngineParameterizedFixture.inl"
#include "graphics2/diligent/resource/GlobalEnvironmentResource.h"
#include "graphics2/frontend/subsystem/CameraRenderState.h"

class GlobalEnvironmentResourceTest : public DiligentEngineParameterizedFixture
{
    protected:
        Diligent::RefCntAutoPtr<Diligent::IPipelineResourceSignature> signature;
        Diligent::RefCntAutoPtr<Diligent::IShaderResourceBinding> srb;
        std::unique_ptr<nc::graphics::GlobalEnvironmentResource> uut;

        void SetUp() override
        {
            INITIALIZE_DILIGENT_FIXTURE;
            constexpr auto variableName = "testTexture";
            const auto resource = nc::graphics::GlobalEnvironmentResource::MakeResourceDesc(variableName);
            auto desc = Diligent::PipelineResourceSignatureDesc{};
            desc.Resources = &resource;
            desc.NumResources = 1;
            desc.BindingIndex = 0;
            engine->GetDevice().CreatePipelineResourceSignature(desc, &signature);
            signature->CreateShaderResourceBinding(&srb);
            uut = std::make_unique<nc::graphics::GlobalEnvironmentResource>(
                *srb->GetVariableByName(Diligent::SHADER_TYPE_PIXEL, variableName),
                engine->GetDevice(),
                engine->GetContext()
            );
        }

        void TearDown() override
        {
            FailIfHasErrorOutput();
        }
};

INSTANTIATE_TEST_SUITE_P(AllApis, GlobalEnvironmentResourceTest, g_apiParams);

TEST_P(GlobalEnvironmentResourceTest, Constructor_initializedVariable)
{
    const auto& var = uut->GetShaderVariable();
    EXPECT_EQ(Diligent::SHADER_RESOURCE_VARIABLE_TYPE_DYNAMIC, var.GetType());
    auto actualResourceDesc = Diligent::ShaderResourceDesc{};
    var.GetResourceDesc(actualResourceDesc);
    EXPECT_EQ(Diligent::SHADER_RESOURCE_TYPE_CONSTANT_BUFFER, actualResourceDesc.Type);

    const auto object = var.Get();
    ASSERT_NE(nullptr, object);
    ASSERT_STREQ(nc::graphics::GlobalEnvironmentResource::UniformBufferName, object->GetDesc().Name);

    const auto actualBuffer = static_cast<Diligent::IBuffer*>(object);
    EXPECT_NE(0, Diligent::RESOURCE_STATE_CONSTANT_BUFFER & actualBuffer->GetState());
}

TEST_P(GlobalEnvironmentResourceTest, Update_succeeds)
{
    auto cameraState = nc::graphics::CameraRenderState{
        .viewProjection = DirectX::XMMatrixPerspectiveFovRH(90.0f, 16.0f / 9.0f, 0.1f, 100.0f)
    };

    // Buffer is dynamic/cpu write only, so we don't have a way of inspecting actual contents.
    // Just expect no error output.
    EXPECT_NO_THROW(uut->Update(cameraState, engine->GetContext()));
    cameraState.viewProjection = DirectX::XMMatrixIdentity();
    EXPECT_NO_THROW(uut->Update(cameraState, engine->GetContext()));
}
