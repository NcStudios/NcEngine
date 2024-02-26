#include "NcGraphicsImpl.h"
#include "PerFrameRenderState.h"
#include "graphics/GraphicsConstants.h"
#include "graphics/GraphicsUtilities.h"
#include "shader_resource/ShaderResourceBus.h"
#include "window/WindowImpl.h"
#include "ncengine/asset/NcAsset.h"
#include "ncengine/config/Config.h"
#include "ncengine/ecs/Ecs.h"
#include "ncengine/ecs/View.h"
#include "ncengine/graphics/WireframeRenderer.h"
#include "ncengine/scene/NcScene.h"
#include "ncengine/task/TaskGraph.h"
#include "ncengine/utility/Log.h"

#include "optick.h"

namespace
{
    struct NcGraphicsStub : nc::graphics::NcGraphics
    {
        NcGraphicsStub(nc::Registry*) {}

        void SetCamera(nc::graphics::Camera*) noexcept override {}
        auto GetCamera() noexcept -> nc::graphics::Camera* override { return nullptr; }
        void SetUi(nc::ui::IUI*) noexcept override {}
        bool IsUiHovered() const noexcept override { return false; }
        void SetSkybox(const std::string&) override {}
        void ClearEnvironment() override {}
    };
} // anonymous namespace

namespace nc::graphics
{
    PostProcessResources::PostProcessResources(ShaderResourceBus* resourceBus, uint32_t maxPointLights)
        : shadowMaps{resourceBus->CreatePPImageArrayBuffer(PostProcessImageType::ShadowMap, maxPointLights, ShaderStage::Fragment, 3u, 0u)},
          maxShadows{maxPointLights}
    {}

    AssetResourcesConfig::AssetResourcesConfig(const config::MemorySettings& memorySettings)
        : maxTextures{memorySettings.maxTextures},
          maxCubeMaps{memorySettings.maxTextures}{} // Todo

    AssetResources::AssetResources(AssetResourcesConfig config, ShaderResourceBus* resourceBus, ModuleProvider modules)
        : meshes{resourceBus->CreateMeshArrayBuffer()},
          onMeshArrayBufferUpdate{modules.Get<asset::NcAsset>()->OnMeshUpdate().Connect(this, &AssetResources::ForwardMeshAssetData)},
          cubeMaps{resourceBus->CreateCubeMapArrayBuffer(config.maxCubeMaps, ShaderStage::Fragment, 4, 1)},
          onCubeMapArrayBufferUpdate{modules.Get<asset::NcAsset>()->OnCubeMapUpdate().Connect(this, &AssetResources::ForwardCubeMapAssetData)},
          textures{resourceBus->CreateTextureArrayBuffer(config.maxTextures, ShaderStage::Fragment, 2, 1)},
          onTextureArrayBufferUpdate{modules.Get<asset::NcAsset>()->OnTextureUpdate().Connect(this, &AssetResources::ForwardTextureAssetData)}{}

    void AssetResources::ForwardMeshAssetData(const asset::MeshUpdateEventData& assetData)
    {
        meshes.Initialize(assetData.vertices, assetData.indices);
    }

    void AssetResources::ForwardTextureAssetData(const asset::TextureUpdateEventData& assetData)
    {
        switch (assetData.updateAction)
        {
            case asset::UpdateAction::Load:
            {
                textures.Add(assetData.data);
                break;
            }
            case asset::UpdateAction::Unload:
            {
                textures.Remove(assetData.data);
                break;
            }
            case asset::UpdateAction::UnloadAll:
            {
                textures.Clear();
                break;
            }
        }
    }

    void AssetResources::ForwardCubeMapAssetData(const asset::CubeMapUpdateEventData& assetData)
    {
        switch (assetData.updateAction)
        {
            case asset::UpdateAction::Load:
            {
                cubeMaps.Add(assetData.data);
                break;
            }
            case asset::UpdateAction::Unload:
            {
                cubeMaps.Remove(assetData.data);
                break;
            }
            case asset::UpdateAction::UnloadAll:
            {
                cubeMaps.Clear();
                break;
            }
        }
    }

    SystemResourcesConfig::SystemResourcesConfig(const config::GraphicsSettings& graphicsSettings, const config::MemorySettings& memorySettings)
        : maxPointLights{memorySettings.maxPointLights},
          maxRenderers{memorySettings.maxRenderers},
          maxSkeletalAnimations{memorySettings.maxSkeletalAnimations},
          maxTextures{memorySettings.maxTextures},
          useShadows{graphicsSettings.useShadows}
    {}

    SystemResources::SystemResources(SystemResourcesConfig config, 
                                    Registry* registry,
                                    ShaderResourceBus* resourceBus,
                                    ModuleProvider modules,
                                    SystemEvents& events)
        : cameras{},
          environment{resourceBus},
          objects{resourceBus, config.maxRenderers},
          pointLights{resourceBus, config.maxPointLights, config.useShadows},
          skeletalAnimations{registry, resourceBus, config.maxSkeletalAnimations, modules.Get<asset::NcAsset>()->OnSkeletalAnimationUpdate(), modules.Get<asset::NcAsset>()->OnBoneUpdate()},
          widgets{},
          ui{registry->GetEcs(), modules, events}
    {
    }

    auto BuildGraphicsModule(const config::ProjectSettings& projectSettings,
                             const config::GraphicsSettings& graphicsSettings,
                             const config::MemorySettings& memorySettings,
                             ModuleProvider modules,
                             Registry* registry,
                             SystemEvents& events,
                             window::WindowImpl* window) -> std::unique_ptr<NcGraphics>
    {
        if (graphicsSettings.enabled)
        {
            auto ncAsset = modules.Get<asset::NcAsset>();
            NC_ASSERT(ncAsset, "NcGraphics requires NcAsset to be registered before it.");
            NC_ASSERT(modules.Get<NcScene>(), "NcGraphics requires NcScene to be registered before it.");

            NC_LOG_TRACE("Selecting Graphics API");
            auto resourceBus = ShaderResourceBus{};
            auto graphicsApi = GraphicsFactory(projectSettings, graphicsSettings, memorySettings, ncAsset, resourceBus, registry, window);

            NC_LOG_TRACE("Building NcGraphics module");
            return std::make_unique<NcGraphicsImpl>(graphicsSettings, memorySettings, registry, modules, events, std::move(graphicsApi), std::move(resourceBus), window);
        }

        NC_LOG_TRACE("Graphics disabled - building NcGraphics stub");
        return std::make_unique<NcGraphicsStub>(registry);
    }

    NcGraphicsImpl::NcGraphicsImpl(const config::GraphicsSettings& graphicsSettings,
                                   const config::MemorySettings& memorySettings,
                                   Registry* registry,
                                   ModuleProvider modules,
                                   SystemEvents& events,
                                   std::unique_ptr<IGraphics> graphics,
                                   ShaderResourceBus shaderResourceBus,
                                   window::WindowImpl* window)
        : m_registry{registry},
          m_graphics{std::move(graphics)},
          m_shaderResourceBus{std::move(shaderResourceBus)},
          m_postProcessResources{&m_shaderResourceBus, memorySettings.maxPointLights},
          m_assetResources{AssetResourcesConfig{memorySettings}, &m_shaderResourceBus, modules},
          m_systemResources{SystemResourcesConfig{graphicsSettings, memorySettings}, m_registry, &m_shaderResourceBus, modules, events},
          m_particleEmitterSystem{ registry, std::bind_front(&NcGraphics::GetCamera, this) }
    {
        window->BindGraphicsOnResizeCallback(std::bind_front(&NcGraphicsImpl::OnResize, this));
        m_graphics->CommitResourceLayout();
    }

    void NcGraphicsImpl::SetCamera(Camera* camera) noexcept
    {
        NC_LOG_TRACE("Setting main camera to: {}", static_cast<void*>(camera));
        m_systemResources.cameras.Set(camera);
    }

    auto NcGraphicsImpl::GetCamera() noexcept -> Camera*
    {
        return m_systemResources.cameras.Get();
    }

    void NcGraphicsImpl::SetUi(ui::IUI* ui) noexcept
    {
        NC_LOG_TRACE("Setting UI to {}", static_cast<void*>(ui));
        m_systemResources.ui.SetClientUI(ui);
    }

    bool NcGraphicsImpl::IsUiHovered() const noexcept
    {
        return m_systemResources.ui.IsHovered();
    }

    void NcGraphicsImpl::SetSkybox(const std::string& path)
    {
        NC_LOG_TRACE("Setting skybox to {}", path);
        m_systemResources.environment.SetSkybox(path);
    }

    void NcGraphicsImpl::ClearEnvironment()
    {
        m_systemResources.environment.Clear();
    }

    void NcGraphicsImpl::Clear() noexcept
    {
        /** @note Don't clear the camera as it may be on a persistent entity. */
        /** @todo graphics::clear not marked noexcept */
        m_particleEmitterSystem.Clear();
        m_graphics->Clear();
        m_systemResources.cameras.Clear();
        m_systemResources.environment.Clear();
        m_systemResources.skeletalAnimations.Clear();
    }

    void NcGraphicsImpl::OnBuildTaskGraph(task::TaskGraph& graph)
    {
        NC_LOG_TRACE("Building NcGraphics workload");

        graph.Add(task::ExecutionPhase::Render, "NcGraphics", [this]{ Run(); });
        graph.Add(task::ExecutionPhase::Free, "ParticleEmitterSystem", [this]{ m_particleEmitterSystem.Run(); });
        graph.Add(task::ExecutionPhase::PostFrameSync, "ProcessParticleFrameEvents", [this]{ m_particleEmitterSystem.ProcessFrameEvents(); } );
    }

    void NcGraphicsImpl::Run()
    {
        OPTICK_CATEGORY("Render", Optick::Category::Rendering);
        if (!m_graphics->FrameBegin())
        {
            return;
        }

        auto currentFrameIndex = m_graphics->CurrentFrameIndex();
        m_assetResources.meshes.Bind(currentFrameIndex);
        auto cameraState = m_systemResources.cameras.Execute(m_registry);
        m_systemResources.ui.Execute(ecs::Ecs(m_registry->GetImpl()));
        auto widgetState = m_systemResources.widgets.Execute(m_registry->GetEcs());
        auto environmentState = m_systemResources.environment.Execute(cameraState, currentFrameIndex);
        auto skeletalAnimationState = m_systemResources.skeletalAnimations.Execute(currentFrameIndex);
        auto objectState = m_systemResources.objects.Execute(currentFrameIndex,
                                                             MultiView<MeshRenderer, Transform>{m_registry},
                                                             MultiView<ToonRenderer, Transform>{m_registry},
                                                             cameraState,
                                                             environmentState,
                                                             skeletalAnimationState);
        auto lightingState = m_systemResources.pointLights.Execute(currentFrameIndex, MultiView<PointLight, Transform>{m_registry});
        auto state = PerFrameRenderState
        {
            std::move(cameraState),
            std::move(environmentState),
            std::move(objectState),
            std::move(lightingState),
            std::move(widgetState),
            m_particleEmitterSystem.GetParticles()
        };

        m_graphics->Draw(state);

        m_postProcessResources.shadowMaps.Update(m_postProcessResources.maxShadows, currentFrameIndex);

        m_graphics->FrameEnd();
    }

    void NcGraphicsImpl::OnResize(float width, float height, bool isMinimized)
    {
        m_systemResources.cameras.Get()->UpdateProjectionMatrix(width, height);
        m_graphics->OnResize(width, height, isMinimized);
    }
} // namespace nc::graphics
