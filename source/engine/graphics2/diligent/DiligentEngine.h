#pragma once

#include "ncengine/graphics/NcGraphics.h"
#include "Common/interface/RefCntAutoPtr.hpp"
#include "Graphics/GraphicsEngine/interface/DeviceContext.h"
#include "Graphics/GraphicsEngine/interface/GraphicsTypes.h"
#include "Graphics/GraphicsEngine/interface/RenderDevice.h"
#include "Graphics/GraphicsEngine/interface/SwapChain.h"

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
        DiligentEngine(const config::GraphicsSettings& graphicsSettings, window::NcWindow& window, std::span<const std::string_view> supportedApis);
        ~DiligentEngine() noexcept;

        auto Device()    -> Diligent::IRenderDevice*  { return m_pDevice.RawPtr(); }
        auto Context()   -> Diligent::IDeviceContext* { return m_pImmediateContext.RawPtr(); }
        auto SwapChain() -> Diligent::ISwapChain*     { return m_pSwapChain.RawPtr(); }

    private:
        Diligent::RefCntAutoPtr<Diligent::IRenderDevice>  m_pDevice;
        Diligent::RefCntAutoPtr<Diligent::IDeviceContext> m_pImmediateContext;
        Diligent::RefCntAutoPtr<Diligent::ISwapChain>     m_pSwapChain;
        std::string_view                                  m_renderApi;
};
} // namespace graphics
} // namespace nc
