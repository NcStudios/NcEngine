#pragma once

#include "ncengine/graphics/NcGraphics.h"
#include "ncengine/module/ModuleProvider.h"

#include "EngineFactoryD3D12.h"
#include "EngineFactoryOpenGL.h"
#include "RenderDevice.h"
#include "DeviceContext.h"
#include "SwapChain.h"
#include "RefCntAutoPtr.hpp"

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
        void OnResize(const Vector2& dimensions, bool isMinimized);

    private:
        Registry* m_registry;
        Connection<const Vector2&, bool> m_onResizeConnection;
        Diligent::RefCntAutoPtr<IRenderDevice> m_pDevice;
        Diligent::RefCntAutoPtr<IDeviceContext> m_pImmediateContext;
        Diligent::RefCntAutoPtr<ISwapChain> m_pPSO;
        Diligent::RENDER_DEVICE_TYPE m_DeviceType = RENDER_DEVICE_TYPE_D3D12;
    };
} // namespace graphics
} // namespace nc
