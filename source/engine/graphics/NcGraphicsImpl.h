#pragma once

#include "IGraphics.h"
#include "ncengine/graphics/NcGraphics.h"
#include "ncengine/module/ModuleProvider.h"
#include "shader_resource/CubeMapArrayBufferHandle.h"
#include "shader_resource/MeshArrayBufferHandle.h"
#include "shader_resource/PPImageArrayBufferHandle.h"
#include "shader_resource/TextureArrayBufferHandle.h"
#include "system/CameraSystem.h"
#include "system/EnvironmentSystem.h"
#include "system/ObjectSystem.h"
#include "system/ParticleEmitterSystem.h"
#include "system/PointLightSystem.h"
#include "system/SkeletalAnimationSystem.h"
#include "system/UISystem.h"
#include "system/WidgetSystem.h"

#include <memory>

namespace nc
{
struct SystemEvents;
class Scene;

namespace config
{
struct GraphicsSettings;
struct MemorySettings;
struct ProjectSettings;
} // namespace config

namespace window
{
class WindowImpl;
} // namespace window

namespace graphics
{
struct ShaderResourceBus;

struct PostProcessResources
{
    PostProcessResources(ShaderResourceBus* resourceBus, uint32_t maxPointLights);
    PPImageArrayBufferHandle shadowMaps;
    uint32_t maxShadows;
};

struct AssetResourcesConfig
{
    AssetResourcesConfig(const config::MemorySettings& memorySettings);
    uint32_t maxTextures;
    uint32_t maxCubeMaps;
};

struct AssetResources
{
    AssetResources(AssetResourcesConfig config, ShaderResourceBus* resourceBus, ModuleProvider modules);

    MeshArrayBufferHandle meshes;
    nc::Connection<const asset::MeshUpdateEventData&> onMeshArrayBufferUpdate;
    void ForwardMeshAssetData(const asset::MeshUpdateEventData& assetData);

    CubeMapArrayBufferHandle cubeMaps;
    nc::Connection<const asset::CubeMapUpdateEventData&> onCubeMapArrayBufferUpdate;
    void ForwardCubeMapAssetData(const asset::CubeMapUpdateEventData& assetData);

    TextureArrayBufferHandle textures;
    nc::Connection<const asset::TextureUpdateEventData&> onTextureArrayBufferUpdate;
    void ForwardTextureAssetData(const asset::TextureUpdateEventData& assetData);
};

struct SystemResourcesConfig
{
    SystemResourcesConfig(const config::GraphicsSettings& graphicsSettings,
                          const config::MemorySettings& memorySettings);
    uint32_t maxPointLights;
    uint32_t maxRenderers;
    uint32_t maxSkeletalAnimations;
    uint32_t maxTextures;
    bool useShadows;
};

struct SystemResources
{
    SystemResources(SystemResourcesConfig config, 
                    Registry* registry,
                    ShaderResourceBus* resourceBus,
                    ModuleProvider modules,
                    SystemEvents& events);
    CameraSystem cameras;
    EnvironmentSystem environment;
    ObjectSystem objects;
    PointLightSystem pointLights;
    SkeletalAnimationSystem skeletalAnimations;
    WidgetSystem widgets;
    UISystem ui;
};

// TODO #340: Window should be moved inside graphics instead of being passed here
auto BuildGraphicsModule(const config::ProjectSettings& projectSettings,
                         const config::GraphicsSettings& graphicsSettings,
                         const config::MemorySettings& memorySettings,
                         ModuleProvider modules,
                         Registry* registry,
                         SystemEvents& events,
                         window::WindowImpl* window) -> std::unique_ptr<NcGraphics>;

class NcGraphicsImpl : public NcGraphics
{
    public:
        NcGraphicsImpl(const config::GraphicsSettings& graphicsSettings,
                       const config::MemorySettings& memorySettings,
                       Registry* registry,
                       ModuleProvider modules,
                       SystemEvents& events,
                       std::unique_ptr<IGraphics> graphics,
                       ShaderResourceBus shaderResourceBus,
                       window::WindowImpl* window);

        void SetCamera(Camera* camera) noexcept override;
        auto GetCamera() noexcept -> Camera* override;
        void SetUi(ui::IUI* ui) noexcept override;
        bool IsUiHovered() const noexcept override;
        void SetSkybox(const std::string& path) override;
        void ClearEnvironment() override;
        void OnBuildTaskGraph(task::TaskGraph&) override;
        void Clear() noexcept override;
        void Run();

        void OnResize(float width, float height, bool isMinimized);

    private:
        Registry* m_registry;
        std::unique_ptr<IGraphics> m_graphics;
        ShaderResourceBus m_shaderResourceBus;
        PostProcessResources m_postProcessResources;
        AssetResources m_assetResources;
        SystemResources m_systemResources;
        ParticleEmitterSystem m_particleEmitterSystem;
    };
} // namespace graphics
} // namespace nc
