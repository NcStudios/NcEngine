#include "application.h"
#include "Assets.h"
#include "engine_context.h"
#include "graphics/resources/CubeMapManager.h"
#include "graphics/resources/ObjectDataManager.h"
#include "graphics/resources/PointLightManager.h"
#include "graphics/resources/EnvironmentDataManager.h"
#include "graphics/resources/ShadowMapManager.h"
#include "graphics/resources/TextureManager.h"

namespace
{
    template<nc::graphics::ShaderResource T>
    class ShaderResourceServiceStub : public nc::graphics::IShaderResourceService<T>
    {
        public:
            using data_type = T;

            void Initialize() override {}
            void Update(const std::vector<data_type>&) override {}
            auto GetDescriptorSet() -> vk::DescriptorSet* override { return nullptr; }
            auto GetDescriptorSetLayout() -> vk::DescriptorSetLayout* override { return nullptr; }
            void Reset() override {}
    };

    template<nc::AssetType T, class InputType>
    class AssetServiceStub : public nc::IAssetService<T, InputType>
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

    class service_stub : public nc::context_stub
    {
        ShaderResourceServiceStub<nc::graphics::ObjectData> objectDataService;
        ShaderResourceServiceStub<nc::PointLightInfo> pointLightInfoService;
        ShaderResourceServiceStub<nc::graphics::Texture> textureService;
        ShaderResourceServiceStub<nc::graphics::ShadowMap> shadowMapService;
        ShaderResourceServiceStub<nc::EnvironmentData> environmentDataService;
        ShaderResourceServiceStub<nc::graphics::CubeMap> cubeMapService;

        AssetServiceStub<nc::AudioClipView, std::string> audioAssetService;
        AssetServiceStub<nc::ConcaveColliderView, std::string> concaveColliderAssetService;
        AssetServiceStub<nc::ConvexHullView, std::string> convexHullAssetService;
        AssetServiceStub<nc::CubeMapView, std::string> cubeMapAssetService;
        AssetServiceStub<nc::MeshView, std::string> meshAssetService;
        AssetServiceStub<nc::TextureView, std::string> textureAssetService;
    };
}

namespace nc
{
    void attach_application(engine_context* context, MainCamera* mainCamera, bool useModule)
    {
        if(useModule)
        {
            context->application = std::make_unique<application>(mainCamera);
        }
        else
        {
            context->application = nullptr;
            context->stubs.push_back(std::make_unique<service_stub>());
        }
    }

    application::application(MainCamera* camera)
        : m_window{},
            m_graphics{camera, m_window.GetHWND(), m_window.GetHINSTANCE(), m_window.GetDimensions()},
            m_ui{m_window.GetHWND()}
    {
        m_graphics.InitializeUI();
        m_window.BindGraphicsOnResizeCallback(std::bind_front(&graphics::Graphics::OnResize, &m_graphics));
        m_window.BindGraphicsSetClearColorCallback(std::bind_front(&graphics::Graphics::SetClearColor, &m_graphics));
        m_window.BindUICallback(std::bind_front(&ui::UISystemImpl::WndProc, &m_ui));
    }

    void application::process_system_messages()
    {
        m_window.ProcessSystemMessages();
    }

    bool application::frame_begin()
    {
        if(m_graphics.FrameBegin())
        {
            m_ui.FrameBegin();
            return true;
        }

        return false;
    }

    void application::draw(const graphics::PerFrameRenderState& state)
    {
        m_graphics.Draw(state);
    }

    void application::frame_end()
    {
        m_graphics.FrameEnd();
    }

    void application::clear()
    {
        m_graphics.Clear();
    }

    void application::register_shutdown_callback(std::function<void()> cb)
    {
        m_window.BindEngineDisableRunningCallback(std::move(cb));
    }

    auto application::get_hwnd() -> HWND
    {
        return m_window.GetHWND();
    }

    auto application::get_graphics() -> graphics::Graphics*
    {
        return &m_graphics;
    }

    auto application::get_ui() -> ui::UISystemImpl*
    {
        return &m_ui;
    }

    auto application::get_window() -> window::WindowImpl*
    {
        return &m_window;
    }
}