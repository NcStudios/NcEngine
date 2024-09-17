#pragma once

// Diligent
#include "Common/interface/RefCntAutoPtr.hpp"
#include "DeviceContext.h"
#include "GraphicsTypes.h"
#include "RenderDevice.h"
#include "SwapChain.h"

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
        DiligentEngine(std::string_view targetApi, window::NcWindow& window);
        ~DiligentEngine() noexcept;

        auto Device()    -> Diligent::IRenderDevice*  { return m_pDevice.RawPtr(); }
        auto Context()   -> Diligent::IDeviceContext* { return m_pImmediateContext.RawPtr(); }
        auto SwapChain() -> Diligent::ISwapChain*     { return m_pSwapChain.RawPtr(); }

    private:
        Diligent::RefCntAutoPtr<Diligent::IRenderDevice>  m_pDevice;
        Diligent::RefCntAutoPtr<Diligent::IDeviceContext> m_pImmediateContext;
        Diligent::RefCntAutoPtr<Diligent::ISwapChain>     m_pSwapChain;
        Diligent::RENDER_DEVICE_TYPE                      m_deviceType = Diligent::RENDER_DEVICE_TYPE_VULKAN;
};
} // namespace graphics
} // namespace nc
