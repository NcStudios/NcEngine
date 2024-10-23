#include "DiligentEngineParameterizedFixture.inl"
#include "graphics2/diligent/resource/GlobalTextureBufferResource.h"

#include "ncengine/asset/AssetData.h"
#include "ncasset/Assets.h"

#include "Graphics/GraphicsEngine/interface/ShaderResourceVariable.h"

#include <array>
#include <string_view>
#include <span>

class GlobalTextureBufferResourceTest : public DiligentEngineParameterizedFixture
{
    protected:
        static constexpr auto maxTextures = 3u;

        Diligent::RefCntAutoPtr<Diligent::IPipelineResourceSignature> signature;
        Diligent::RefCntAutoPtr<Diligent::IShaderResourceBinding> srb;
        std::unique_ptr<nc::graphics::GlobalTextureBufferResource> uut;

        void SetUp() override
        {
            INITIALIZE_DILIGENT_FIXTURE;
            constexpr auto variableName = "testTexture";
            const auto resource = nc::graphics::GlobalTextureBufferResource::MakeResourceDesc(variableName, maxTextures);
            const auto sampler = nc::graphics::GlobalTextureBufferResource::MakeSamplerDesc(variableName);
            auto desc = Diligent::PipelineResourceSignatureDesc{};
            desc.Resources = &resource;
            desc.NumResources = 1;
            desc.ImmutableSamplers = &sampler;
            desc.NumImmutableSamplers = 1;
            desc.BindingIndex = 0;
            desc.UseCombinedTextureSamplers = true;
            engine->GetDevice().CreatePipelineResourceSignature(desc, &signature);
            signature->CreateShaderResourceBinding(&srb);
            uut = std::make_unique<nc::graphics::GlobalTextureBufferResource>(
                *srb->GetVariableByName(resource.ShaderStages, variableName),
                maxTextures
            );
        }

        void TearDown() override
        {
            FailIfHasErrorOutput();
        }

        auto GetTextureView(uint32_t index) -> Diligent::ITextureView*
        {
            return static_cast<Diligent::ITextureView*>(uut->GetShaderVariable().Get(index));
        }
};

INSTANTIATE_TEST_SUITE_P(AllApis, GlobalTextureBufferResourceTest, g_apiParams);

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

TEST_P(GlobalTextureBufferResourceTest, Load_singleTexture_succeeds)
{
    const auto& expectedTexture = imageTexture1;
    uut->Load(std::span{&expectedTexture, 1}, engine->GetContext(), engine->GetDevice());
    const auto actualView = GetTextureView(0);
    const auto endOfRange = GetTextureView(1);
    ASSERT_NE(actualView, nullptr);
    EXPECT_EQ(endOfRange, nullptr);

    const auto actualTexture = actualView->GetTexture();
    const auto desc = actualTexture->GetDesc();
    EXPECT_EQ(Diligent::RESOURCE_STATE_SHADER_RESOURCE, actualTexture->GetState());
    EXPECT_EQ(expectedTexture.texture.width, desc.GetWidth());
    EXPECT_EQ(expectedTexture.texture.height, desc.GetHeight());
    EXPECT_EQ(Diligent::TEX_FORMAT_RGBA8_UNORM_SRGB, desc.Format);
}

TEST_P(GlobalTextureBufferResourceTest, Load_normalMap_selectsCorrectFormat)
{
    const auto& expectedTexture = normalTexture;
    uut->Load(std::span{&expectedTexture, 1}, engine->GetContext(), engine->GetDevice());
    const auto loadedView = GetTextureView(0);
    ASSERT_NE(loadedView, nullptr);

    const auto actualTexture = loadedView->GetTexture();
    const auto desc = actualTexture->GetDesc();
    EXPECT_EQ(Diligent::TEX_FORMAT_RGBA8_UNORM, desc.Format);
}

TEST_P(GlobalTextureBufferResourceTest, Load_existingTextures_appendsToArray)
{
    const auto& expectedTexture1 = imageTexture1;
    const auto& expectedTexture2 = imageTexture2;
    uut->Load(std::span{&expectedTexture1, 1}, engine->GetContext(), engine->GetDevice());
    uut->Load(std::span{&expectedTexture2, 1}, engine->GetContext(), engine->GetDevice());

    const auto firstView = GetTextureView(0);
    const auto secondView = GetTextureView(1);
    const auto endOfRange = GetTextureView(2);
    ASSERT_NE(firstView, nullptr);
    ASSERT_NE(secondView, nullptr);
    EXPECT_EQ(endOfRange, nullptr);

    const auto firstDesc = firstView->GetTexture()->GetDesc();
    const auto secondDesc = secondView->GetTexture()->GetDesc();
    EXPECT_EQ(expectedTexture1.texture.width, firstDesc.GetWidth());
    EXPECT_EQ(expectedTexture1.texture.height, firstDesc.GetHeight());
    EXPECT_EQ(expectedTexture2.texture.width, secondDesc.GetWidth());
    EXPECT_EQ(expectedTexture2.texture.height, secondDesc.GetHeight());
}

TEST_P(GlobalTextureBufferResourceTest, Load_exceedsMaxTextures_throws)
{
    const auto textures = std::array{imageTexture1, imageTexture2, normalTexture};
    uut->Load(textures, engine->GetContext(), engine->GetDevice());
    EXPECT_THROW(uut->Load(textures, engine->GetContext(), engine->GetDevice()), std::exception);
}

TEST_P(GlobalTextureBufferResourceTest, Load_afterUnload_overwritesExisting)
{
    auto initialTextures = std::array{imageTexture1, imageTexture2};
    auto overwriteTextures = std::array{normalTexture};
    uut->Load(initialTextures, engine->GetContext(), engine->GetDevice());
    uut->Unload();
    uut->Load(overwriteTextures, engine->GetContext(), engine->GetDevice());

    auto overwrittenView = GetTextureView(0);
    ASSERT_NE(overwrittenView, nullptr);

    const auto overwrittenTexture = overwrittenView->GetTexture();
    const auto overwrittenDesc = overwrittenTexture->GetDesc();
    EXPECT_EQ(Diligent::RESOURCE_STATE_SHADER_RESOURCE, overwrittenTexture->GetState());
    EXPECT_EQ(normalTexture.texture.width, overwrittenDesc.GetWidth());
    EXPECT_EQ(normalTexture.texture.height, overwrittenDesc.GetHeight());
}
