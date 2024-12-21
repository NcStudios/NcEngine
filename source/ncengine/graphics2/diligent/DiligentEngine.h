#pragma once

#include "ShaderFactory.h"

#include "Common/interface/RefCntAutoPtr.hpp"
#include "Graphics/GraphicsEngine/interface/DeviceContext.h"
#include "Graphics/GraphicsEngine/interface/GraphicsTypes.h"
#include "Graphics/GraphicsEngine/interface/RenderDevice.h"
#include "Graphics/GraphicsEngine/interface/SwapChain.h"

#include <memory>

struct GLFWwindow;

namespace nc::graphics
{
struct DeviceCapability
{
    uint32_t msaaSampleCount = 1u;
};

class DiligentEngine
{
    public:
        DiligentEngine(const Diligent::EngineCreateInfo& engineCreateInfo,
                       DeviceCapability requestedCaps,
                       GLFWwindow* windowHandle,
                       std::string_view shadersPath,
                       Diligent::DebugMessageCallbackType logCallback = nullptr);
        ~DiligentEngine() noexcept;

        auto GetDevice()           -> Diligent::IRenderDevice&  { return *m_pDevice; }
        auto GetContext()          -> Diligent::IDeviceContext& { return *m_pImmediateContext; }
        auto GetSwapChain()        -> Diligent::ISwapChain&     { return *m_pSwapChain; }
        auto GetShaderFactory()    -> ShaderFactory&            { return *m_shaderFactory; }
        auto GetDeviceCapability() -> const DeviceCapability&   { return m_deviceCapability; }

    private:
        Diligent::RefCntAutoPtr<Diligent::IRenderDevice>  m_pDevice;
        Diligent::RefCntAutoPtr<Diligent::IDeviceContext> m_pImmediateContext;
        Diligent::RefCntAutoPtr<Diligent::ISwapChain>     m_pSwapChain;
        std::unique_ptr<ShaderFactory>                    m_shaderFactory;
        DeviceCapability                                  m_deviceCapability;
};
} // namespace nc::graphics
