#include "DiligentEngineFixture.inl"
#include "graphics2/diligent/resource/CubeMapBufferResource.h"
#include "graphics2/diligent/resource/ResourceTypes.h"

#include "ncengine/asset/AssetData.h"
#include "ncasset/Assets.h"

#include "Graphics/GraphicsEngine/interface/ShaderResourceVariable.h"

#include <array>
#include <string_view>
#include <span>

class CubeMapBufferResourceTest : public DiligentEngineFixture
{
    protected:
        static constexpr auto maxCubeMaps = 3u;

        Diligent::RefCntAutoPtr<Diligent::IPipelineResourceSignature> signature;
        Diligent::RefCntAutoPtr<Diligent::IShaderResourceBinding> srb;
        std::unique_ptr<nc::graphics::CubeMapBufferResource> uut;

        CubeMapBufferResourceTest()
        {
            constexpr auto variableName = "testCubeMap";
            const auto resourceDesc = nc::graphics::CubeMapBufferDesc{
                .resourceKey = variableName,
                .shaderType = Diligent::SHADER_TYPE_PIXEL,
                .maxElementCount = maxCubeMaps
            };
            const auto resource = nc::graphics::ToPipelineResourceDesc(resourceDesc);
            const auto sampler = nc::graphics::CubeMapBufferResource::MakeSamplerDesc(variableName);
            auto desc = Diligent::PipelineResourceSignatureDesc{};
            desc.Resources = &resource;
            desc.NumResources = 1;
            desc.ImmutableSamplers = &sampler;
            desc.NumImmutableSamplers = 1;
            desc.BindingIndex = 0;
            desc.UseCombinedTextureSamplers = true;
            engine->GetDevice().CreatePipelineResourceSignature(desc, &signature);
            signature->CreateShaderResourceBinding(&srb);
            uut = std::make_unique<nc::graphics::CubeMapBufferResource>(
                *srb->GetVariableByName(resource.ShaderStages, variableName),
                maxCubeMaps
            );
        }

        ~CubeMapBufferResourceTest()
        {
            FailIfHasErrorOutput();
        }

        auto GetRenderTargetView(uint32_t index) -> Diligent::ITextureView*
        {
            return static_cast<Diligent::ITextureView*>(uut->GetShaderVariable().Get(index));
        }
};

const auto face1x1 = std::vector<unsigned char>{
    0, 1, 2, 3
};

const auto face2x2 = std::vector<unsigned char>{
    0, 1, 2, 3,  0, 1, 2, 3,
    0, 1, 2, 3,  0, 1, 2, 3
};

const auto face4x4 = std::vector<unsigned char>{
    0, 1, 2, 3,  0, 1, 2, 3,  0, 1, 2, 3,  0, 1, 2, 3,
    0, 1, 2, 3,  0, 1, 2, 3,  0, 1, 2, 3,  0, 1, 2, 3,
    0, 1, 2, 3,  0, 1, 2, 3,  0, 1, 2, 3,  0, 1, 2, 3,
    0, 1, 2, 3,  0, 1, 2, 3,  0, 1, 2, 3,  0, 1, 2, 3
};

const auto imageCubeMap1 = nc::asset::CubeMapWithId{
    .cubeMap = nc::asset::CubeMap{
        .format = nc::asset::TextureFormat::RGBA8_UNORM_SRGB,
        .faceSideLength = 2,
        .faces = {
            face2x2,
            face2x2,
            face2x2,
            face2x2,
            face2x2,
            face2x2
        }
    },
    .id = 0,
};

const auto imageCubeMap2 = nc::asset::CubeMapWithId{
    .cubeMap = nc::asset::CubeMap{
        .format = nc::asset::TextureFormat::RGBA8_UNORM_SRGB,
        .faceSideLength = 4,
        .faces = {
            face4x4,
            face4x4,
            face4x4,
            face4x4,
            face4x4,
            face4x4
        }
    },
    .id = 1,
};

const auto imageCubeMap3 = nc::asset::CubeMapWithId{
    .cubeMap = nc::asset::CubeMap{
        .format = nc::asset::TextureFormat::RGBA8_UNORM_SRGB,
        .faceSideLength = 1,
        .faces = {
            face1x1,
            face1x1,
            face1x1,
            face1x1,
            face1x1,
            face1x1
        }
    },
    .id = 2,
};

TEST_F(CubeMapBufferResourceTest, Load_singleCubeMap_succeeds)
{
    const auto& expectedCubeMap = imageCubeMap1;
    uut->Load(std::span{&expectedCubeMap, 1}, engine->GetContext(), engine->GetDevice());
    const auto actualView = GetRenderTargetView(0);
    ASSERT_NE(actualView, nullptr);

    const auto actualCubeMap = actualView->GetTexture();
    const auto desc = actualCubeMap->GetDesc();
    EXPECT_EQ(Diligent::RESOURCE_STATE_SHADER_RESOURCE, actualCubeMap->GetState());
    EXPECT_EQ(expectedCubeMap.cubeMap.faceSideLength, desc.GetWidth());
    EXPECT_EQ(Diligent::TEX_FORMAT_RGBA8_UNORM_SRGB, desc.Format);
}

TEST_F(CubeMapBufferResourceTest, Load_existingcubeMaps_appendsToArray)
{
    const auto& expectedCubeMap1 = imageCubeMap1;
    const auto& expectedCubeMap2 = imageCubeMap2;
    uut->Load(std::span{&expectedCubeMap1, 1}, engine->GetContext(), engine->GetDevice());
    uut->Load(std::span{&expectedCubeMap2, 1}, engine->GetContext(), engine->GetDevice());

    const auto firstView = GetRenderTargetView(0);
    const auto secondView = GetRenderTargetView(1);
    ASSERT_NE(firstView, nullptr);
    ASSERT_NE(secondView, nullptr);

    const auto firstDesc = firstView->GetTexture()->GetDesc();
    const auto secondDesc = secondView->GetTexture()->GetDesc();
    EXPECT_EQ(expectedCubeMap1.cubeMap.faceSideLength, firstDesc.GetWidth());
    EXPECT_EQ(expectedCubeMap2.cubeMap.faceSideLength, secondDesc.GetWidth());
}

TEST_F(CubeMapBufferResourceTest, Load_exceedsmaxCubeMaps_throws)
{
    const auto cubeMaps = std::array{imageCubeMap1, imageCubeMap2, imageCubeMap3};
    uut->Load(cubeMaps, engine->GetContext(), engine->GetDevice());
    EXPECT_THROW(uut->Load(cubeMaps, engine->GetContext(), engine->GetDevice()), std::exception);
}

TEST_F(CubeMapBufferResourceTest, Load_afterUnload_overwritesExisting)
{
    auto initialCubeMaps = std::array{imageCubeMap1, imageCubeMap2};
    auto overwriteCubeMaps = std::array{imageCubeMap3};
    uut->Load(initialCubeMaps, engine->GetContext(), engine->GetDevice());
    uut->Unload();
    uut->Load(overwriteCubeMaps, engine->GetContext(), engine->GetDevice());

    auto overwrittenView = GetRenderTargetView(0);
    ASSERT_NE(overwrittenView, nullptr);

    const auto overwrittenCubeMap = overwrittenView->GetTexture();
    const auto overwrittenDesc = overwrittenCubeMap->GetDesc();
    EXPECT_EQ(Diligent::RESOURCE_STATE_SHADER_RESOURCE, overwrittenCubeMap->GetState());
    EXPECT_EQ(imageCubeMap3.cubeMap.faceSideLength, overwrittenDesc.GetWidth());
}
