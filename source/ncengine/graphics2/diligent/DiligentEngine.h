#pragma once

#include "ShaderFactory.h"
#include "ncengine/config/Config.h"
#include "ncengine/graphics/NcGraphics.h"

#include "Common/interface/RefCntAutoPtr.hpp"
#include "Graphics/GraphicsEngine/interface/DeviceContext.h"
#include "Graphics/GraphicsEngine/interface/GraphicsTypes.h"
#include "Graphics/GraphicsEngine/interface/RenderDevice.h"
#include "Graphics/GraphicsEngine/interface/SwapChain.h"

#include <memory>

namespace nc
{
namespace window
{
class NcWindow;
} // namespace window

namespace graphics
{
class DiligentEngine
{
    public:
        DiligentEngine(const config::GraphicsSettings& graphicsSettings,
                       const Diligent::EngineCreateInfo& engineCreateInfo,
                       GLFWwindow* window,
                       std::span<const std::string_view> supportedApis,
                       Diligent::DebugMessageCallbackType logCallback = nullptr);
        ~DiligentEngine() noexcept;

        auto GetDevice()        -> Diligent::IRenderDevice&  { return *m_pDevice; }
        auto GetContext()       -> Diligent::IDeviceContext& { return *m_pImmediateContext; }
        auto GetSwapChain()     -> Diligent::ISwapChain&     { return *m_pSwapChain; }
        auto GetShaderFactory() -> ShaderFactory&            { return *m_shaderFactory; }

    private:
        Diligent::RefCntAutoPtr<Diligent::IRenderDevice>  m_pDevice;
        Diligent::RefCntAutoPtr<Diligent::IDeviceContext> m_pImmediateContext;
        Diligent::RefCntAutoPtr<Diligent::ISwapChain>     m_pSwapChain;
        std::unique_ptr<ShaderFactory>                    m_shaderFactory;
        std::string_view                                  m_renderApi;
};
} // namespace graphics
} // namespace nc
