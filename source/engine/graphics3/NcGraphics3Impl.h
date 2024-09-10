#pragma once

#include "ncengine/asset/AssetData.h"
#include "ncengine/asset/NcAsset.h"
#include "ncengine/graphics/NcGraphics.h"
#include "ncengine/module/ModuleProvider.h"

#include "Graphics/GraphicsEngineD3D11/interface/EngineFactoryD3D11.h"
#include "Graphics/GraphicsEngineD3D12/interface/EngineFactoryD3D12.h"
#include "Graphics/GraphicsEngineOpenGL/interface/EngineFactoryOpenGL.h"
#include "Graphics/GraphicsEngineVulkan/interface/EngineFactoryVk.h"

#include "Graphics/GraphicsEngine/interface/RenderDevice.h"
#include "Graphics/GraphicsEngine/interface/DeviceContext.h"
#include "Graphics/GraphicsEngine/interface/SwapChain.h"

#include "Common/interface/RefCntAutoPtr.hpp"

#include <memory>

namespace nc
{
class Scene;

namespace window
{
class NcWindow;
} // namespace window

namespace graphics
{

class NcGraphics3Impl : public NcGraphics
{
    public:
        NcGraphics3Impl(const config::GraphicsSettings& graphicsSettings,
                       const config::MemorySettings& memorySettings,
                       Registry* registry,
                       ModuleProvider modules,
                       SystemEvents& events,
                       window::NcWindow& window);

        ~NcGraphics3Impl() noexcept;

        void SetCamera(Camera* camera) noexcept override;
        auto GetCamera() noexcept -> Camera* override;
        void SetUi(ui::IUI* ui) noexcept override;
        bool IsUiHovered() const noexcept override;
        void SetSkybox(const std::string& path) override;
        void ClearEnvironment() override;
        void OnBuildTaskGraph(task::UpdateTasks& update, task::RenderTasks& render) override;
        void Clear() noexcept override;
        void Run();
        void Update();
        void OnResize(const Vector2& dimensions, bool isMinimized);

    private:
        void SetupTriangle01(std::string_view defaultShadersPath, Diligent::ShaderCreateInfo shaderCI);
        void SetupCube02(std::string_view defaultShadersPath, Diligent::ShaderCreateInfo shaderCI);
        void SetupTexturedCube03(std::string_view defaultShadersPath, Diligent::ShaderCreateInfo shaderCI);

        void UpdateCube02();
        void UpdateTexturedCube03();

        void RenderTriangle01();
        void RenderCube02();
        void RenderTexturedCube03();

        void AddTextures(const asset::TextureUpdateEventData& assetData);

        Registry* m_registry;
        Connection<const Vector2&, bool> m_onResizeConnection;
        Diligent::RefCntAutoPtr<Diligent::IRenderDevice>  m_pDevice;
        Diligent::RefCntAutoPtr<Diligent::IDeviceContext> m_pImmediateContext;
        Diligent::RefCntAutoPtr<Diligent::ISwapChain>     m_pSwapChain;
        Diligent::RefCntAutoPtr<Diligent::IPipelineState> m_pPSO;
        Diligent::RefCntAutoPtr<Diligent::IPipelineState> m_pPSOCube;
        Diligent::RefCntAutoPtr<Diligent::IPipelineState> m_pPSOTextCube;
        Diligent::RefCntAutoPtr<Diligent::IBuffer> m_cubeVertexBuffer;
        Diligent::RefCntAutoPtr<Diligent::IBuffer> m_cubeIndexBuffer;
        Diligent::RefCntAutoPtr<Diligent::IBuffer> m_textCubeVertexBuffer;
        Diligent::RefCntAutoPtr<Diligent::IBuffer> m_textCubeIndexBuffer;
        Diligent::RefCntAutoPtr<Diligent::IBuffer> m_VSConstants;
        Diligent::RENDER_DEVICE_TYPE m_DeviceType = Diligent::RENDER_DEVICE_TYPE_D3D12; /* @todo: Decision based on config and device capabilities */
        Diligent::RefCntAutoPtr<Diligent::IShaderResourceBinding> m_pCubeSRB;
        Diligent::RefCntAutoPtr<Diligent::IShaderResourceBinding> m_pTextCubeSRB;
        Diligent::RefCntAutoPtr<Diligent::ITexture> m_texture;
        Diligent::RefCntAutoPtr<Diligent::ITextureView> m_pTextureSRV;
        DirectX::XMMATRIX m_worldViewProj;

        nc::Connection<const asset::TextureUpdateEventData&> m_onTextureArrayBufferUpdate;
        std::vector<asset::TextureWithId> m_textures;
    };
} // namespace graphics
} // namespace nc
