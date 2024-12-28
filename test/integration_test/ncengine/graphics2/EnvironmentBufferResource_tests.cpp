#include "DiligentEngineFixture.inl"
#include "graphics2/diligent/resource/EnvironmentBufferResource.h"
#include "graphics2/diligent/resource/ResourceTypes.h"
#include "graphics2/frontend/subsystem/CameraRenderState.h"
#include "graphics2/frontend/subsystem/LightRenderState.h"

#include "ncmath/Vector.h"

class EnvironmentBufferResourceTest : public DiligentEngineFixture
{
    protected:
        Diligent::RefCntAutoPtr<Diligent::IPipelineResourceSignature> signature;
        Diligent::RefCntAutoPtr<Diligent::IShaderResourceBinding> srb;
        std::unique_ptr<nc::graphics::EnvironmentBufferResource> uut;

        EnvironmentBufferResourceTest()
        {
            constexpr auto variableName = "EnvironmentDataUniformBuffer";
            const auto resourceDesc = nc::graphics::UniformBufferResourceDesc{
                variableName,
                Diligent::SHADER_TYPE::SHADER_TYPE_PIXEL
            };
            const auto resource = nc::graphics::ToPipelineResourceDesc(resourceDesc);
            auto desc = Diligent::PipelineResourceSignatureDesc{};
            desc.Resources = &resource;
            desc.NumResources = 1;
            desc.BindingIndex = 0;
            engine->GetDevice().CreatePipelineResourceSignature(desc, &signature);
            signature->CreateShaderResourceBinding(&srb);
            uut = std::make_unique<nc::graphics::EnvironmentBufferResource>(
                engine->GetContext(),
                engine->GetDevice(),
                *srb->GetVariableByName(Diligent::SHADER_TYPE_PIXEL, variableName)
            );
        }

        ~EnvironmentBufferResourceTest()
        {
            FailIfHasErrorOutput();
        }
};

TEST_F(EnvironmentBufferResourceTest, Constructor_initializedVariable)
{
    const auto& var = uut->GetShaderVariable();
    EXPECT_EQ(Diligent::SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE, var.GetType());
    auto actualResourceDesc = Diligent::ShaderResourceDesc{};
    var.GetResourceDesc(actualResourceDesc);
    EXPECT_EQ(Diligent::SHADER_RESOURCE_TYPE_CONSTANT_BUFFER, actualResourceDesc.Type);

    const auto object = var.Get();
    ASSERT_NE(nullptr, object);
    ASSERT_STREQ("EnvironmentDataUniformBuffer", object->GetDesc().Name);

    const auto actualBuffer = static_cast<Diligent::IBuffer*>(object);
    EXPECT_NE(0, Diligent::RESOURCE_STATE_CONSTANT_BUFFER & actualBuffer->GetState());
}

TEST_F(EnvironmentBufferResourceTest, Update_succeeds)
{
    auto cameraState = nc::graphics::CameraRenderState{
        .viewProjection = DirectX::XMMatrixPerspectiveFovRH(90.0f, 16.0f / 9.0f, 0.1f, 100.0f),
        .invProjection = DirectX::XMMatrixIdentity(),
        .position = nc::Vector3::Zero(),
        .nearClip = 0.1f,
        .farClip = 400.0f
    };

    auto lightRenderState = nc::graphics::LightRenderState{};

    // Buffer is dynamic/cpu write only, so we don't have a way of inspecting actual contents.
    // Just expect no error output.
    EXPECT_NO_THROW(uut->Update(engine->GetContext(), cameraState, lightRenderState));
    cameraState.viewProjection = DirectX::XMMatrixIdentity();
    EXPECT_NO_THROW(uut->Update(engine->GetContext(), cameraState, lightRenderState));
}
