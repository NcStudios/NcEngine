#pragma once

#include "graphics_interface.h"

#include "resources/CubeMapManager.h"
#include "resources/ObjectDataManager.h"
#include "resources/PointLightManager.h"
#include "resources/EnvironmentDataManager.h"
#include "resources/ShadowMapManager.h"
#include "resources/TextureManager.h"




#include "assets/AudioClipAssetManager.h"
#include "assets/ConcaveColliderAssetManager.h"
#include "assets/ConvexHullAssetManager.h"
#include "assets/CubeMapAssetManager.h"
#include "assets/MeshAssetManager.h"
#include "assets/TextureAssetManager.h"

namespace nc::graphics
{
    template<ShaderResource T>
    class ShaderResourceServiceStub : public IShaderResourceService<T>
    {
        public:
            using data_type = T;
            
            void Initialize() override {}
            void Update(const std::vector<data_type>&) override {}
            auto GetDescriptorSet() -> vk::DescriptorSet* override { return nullptr; }
            auto GetDescriptorSetLayout() -> vk::DescriptorSetLayout* override { return nullptr; }
            void Reset() override {}
    };

    template<AssetType T, class InputType>
    class AssetServiceStub : public IAssetService<T, InputType>
    {
        public:
            using data_type = T;

            bool Load(const InputType&, bool) override { return true; }
            bool Load(std::span<const InputType>, bool) override { return true; }
            bool Unload(const InputType&) override { return true; }
            void UnloadAll() override {}
            auto Acquire(const InputType&) const -> data_type override { return data_type{}; }
            bool IsLoaded(const InputType&) const override { return true; }
    };

    class dummy_graphics : public graphics_interface
    {
        ShaderResourceServiceStub<ObjectData> objectDataService;
        ShaderResourceServiceStub<PointLightInfo> pointLightInfoService;
        ShaderResourceServiceStub<Texture> textureService;
        ShaderResourceServiceStub<ShadowMap> shadowMapService;
        ShaderResourceServiceStub<EnvironmentData> environmentDataService;
        ShaderResourceServiceStub<CubeMap> cubeMapService;

        AssetServiceStub<AudioClipView, std::string> audioAssetService;
        AssetServiceStub<ConcaveColliderView, std::string> concaveColliderAssetService;
        AssetServiceStub<ConvexHullView, std::string> convexHullAssetService;
        AssetServiceStub<CubeMapView, std::string> cubeMapAssetService;
        AssetServiceStub<MeshView, std::string> meshAssetService;
        AssetServiceStub<TextureView, std::string> textureAssetService;


        public:
        void initialize_ui() override {}
        bool frame_begin() override { return false; }
        void draw(const PerFrameRenderState&) override {}
        void frame_end() override {}
        void clear() override {}
        void on_resize(float, float, float, float, WPARAM) override {}
        void set_clear_color(std::array<float, 4>) override {}
    };
}