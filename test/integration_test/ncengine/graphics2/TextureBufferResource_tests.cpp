#include "DiligentEngineFixture.inl"
#include "graphics2/diligent/resource/TextureBufferResource.h"
#include "graphics2/diligent/resource/ResourceTypes.h"

#include "ncengine/asset/AssetData.h"
#include "ncasset/Assets.h"

#include "Graphics/GraphicsEngine/interface/ShaderResourceVariable.h"

#include <array>
#include <string_view>
#include <span>

class TextureBufferResourceTest : public DiligentEngineFixture
{
    protected:
        static constexpr auto maxTextures = 3u;

        Diligent::RefCntAutoPtr<Diligent::IPipelineResourceSignature> signature;
        Diligent::RefCntAutoPtr<Diligent::IShaderResourceBinding> srb;
        std::unique_ptr<nc::graphics::TextureBufferResource> uut;

        TextureBufferResourceTest()
        {
            constexpr auto variableName = "testTexture";
            const auto resourceDesc = nc::graphics::TextureBufferDesc{
                .resourceKey = variableName,
                .shaderType = Diligent::SHADER_TYPE_PIXEL,
                .maxElementCount = maxTextures
            };
            const auto resource = nc::graphics::ToPipelineResourceDesc(resourceDesc);
            const auto sampler = nc::graphics::TextureBufferResource::MakeSamplerDesc(variableName);
            auto desc = Diligent::PipelineResourceSignatureDesc{};
            desc.Resources = &resource;
            desc.NumResources = 1;
            desc.ImmutableSamplers = &sampler;
            desc.NumImmutableSamplers = 1;
            desc.BindingIndex = 0;
            desc.UseCombinedTextureSamplers = true;
            engine->GetDevice().CreatePipelineResourceSignature(desc, &signature);
            signature->CreateShaderResourceBinding(&srb);
            uut = std::make_unique<nc::graphics::TextureBufferResource>(
                *srb->GetVariableByName(resource.ShaderStages, variableName),
                maxTextures
            );
        }

        ~TextureBufferResourceTest()
        {
            FailIfHasErrorOutput();
        }

        auto GetRenderTargetView(uint32_t index) -> Diligent::ITextureView*
        {
            return static_cast<Diligent::ITextureView*>(uut->GetShaderVariable().Get(index));
        }
};

const auto imageTexture1 = nc::asset::TextureWithId{
    .texture = nc::asset::Texture{
        .width = 1,
        .height = 1,
        .pixelData = {0, 1, 2, 3}
    },
    .id = 0,
    .flags = nc::asset::AssetFlags::TextureTypeImage
};

const auto imageTexture2 = nc::asset::TextureWithId{
    .texture = nc::asset::Texture{
        .width = 2,
        .height = 2,
        .pixelData = {
            0, 1, 2, 3,  0, 1, 2, 3,
            0, 1, 2, 3,  0, 1, 2, 3
        }
    },
    .id = 1,
    .flags = nc::asset::AssetFlags::TextureTypeImage
};

const auto normalTexture = nc::asset::TextureWithId{
    .texture = nc::asset::Texture{
        .width = 1,
        .height = 2,
        .pixelData = {
            0, 1, 2, 3,
            0, 1, 2, 3
        }
    },
    .id = 2,
    .flags = nc::asset::AssetFlags::TextureTypeNormalMap
};

TEST_F(TextureBufferResourceTest, Load_singleTexture_succeeds)
{
    const auto& expectedTexture = imageTexture1;
    uut->Load(std::span{&expectedTexture, 1}, engine->GetContext(), engine->GetDevice());
    const auto actualView = GetRenderTargetView(0);
    ASSERT_NE(actualView, nullptr);

    const auto actualTexture = actualView->GetTexture();
    const auto desc = actualTexture->GetDesc();
    EXPECT_EQ(Diligent::RESOURCE_STATE_SHADER_RESOURCE, actualTexture->GetState());
    EXPECT_EQ(expectedTexture.texture.width, desc.GetWidth());
    EXPECT_EQ(expectedTexture.texture.height, desc.GetHeight());
    EXPECT_EQ(Diligent::TEX_FORMAT_RGBA8_UNORM_SRGB, desc.Format);
}

TEST_F(TextureBufferResourceTest, Load_normalMap_selectsCorrectFormat)
{
    const auto& expectedTexture = normalTexture;
    uut->Load(std::span{&expectedTexture, 1}, engine->GetContext(), engine->GetDevice());
    const auto loadedView = GetRenderTargetView(0);
    ASSERT_NE(loadedView, nullptr);

    const auto actualTexture = loadedView->GetTexture();
    const auto desc = actualTexture->GetDesc();
    EXPECT_EQ(Diligent::TEX_FORMAT_RGBA8_UNORM, desc.Format);
}

TEST_F(TextureBufferResourceTest, Load_existingTextures_appendsToArray)
{
    const auto& expectedTexture1 = imageTexture1;
    const auto& expectedTexture2 = imageTexture2;
    uut->Load(std::span{&expectedTexture1, 1}, engine->GetContext(), engine->GetDevice());
    uut->Load(std::span{&expectedTexture2, 1}, engine->GetContext(), engine->GetDevice());

    const auto firstView = GetRenderTargetView(0);
    const auto secondView = GetRenderTargetView(1);
    ASSERT_NE(firstView, nullptr);
    ASSERT_NE(secondView, nullptr);

    const auto firstDesc = firstView->GetTexture()->GetDesc();
    const auto secondDesc = secondView->GetTexture()->GetDesc();
    EXPECT_EQ(expectedTexture1.texture.width, firstDesc.GetWidth());
    EXPECT_EQ(expectedTexture1.texture.height, firstDesc.GetHeight());
    EXPECT_EQ(expectedTexture2.texture.width, secondDesc.GetWidth());
    EXPECT_EQ(expectedTexture2.texture.height, secondDesc.GetHeight());
}

TEST_F(TextureBufferResourceTest, Load_exceedsMaxTextures_throws)
{
    const auto textures = std::array{imageTexture1, imageTexture2, normalTexture};
    uut->Load(textures, engine->GetContext(), engine->GetDevice());
    EXPECT_THROW(uut->Load(textures, engine->GetContext(), engine->GetDevice()), std::exception);
}

TEST_F(TextureBufferResourceTest, Load_afterUnload_overwritesExisting)
{
    auto initialTextures = std::array{imageTexture1, imageTexture2};
    auto overwriteTextures = std::array{normalTexture};
    uut->Load(initialTextures, engine->GetContext(), engine->GetDevice());
    uut->Unload();
    uut->Load(overwriteTextures, engine->GetContext(), engine->GetDevice());

    auto overwrittenView = GetRenderTargetView(0);
    ASSERT_NE(overwrittenView, nullptr);

    const auto overwrittenTexture = overwrittenView->GetTexture();
    const auto overwrittenDesc = overwrittenTexture->GetDesc();
    EXPECT_EQ(Diligent::RESOURCE_STATE_SHADER_RESOURCE, overwrittenTexture->GetState());
    EXPECT_EQ(normalTexture.texture.width, overwrittenDesc.GetWidth());
    EXPECT_EQ(normalTexture.texture.height, overwrittenDesc.GetHeight());
}
