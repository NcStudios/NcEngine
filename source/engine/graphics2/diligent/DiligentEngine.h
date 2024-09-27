#pragma once

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
namespace api
{
    constexpr std::string_view Headless = std::string_view("headless");
    constexpr std::string_view OpenGL   = std::string_view("opengl");
    constexpr std::string_view D3D11    = std::string_view("d3d11");
    constexpr std::string_view D3D12    = std::string_view("d3d12");
    constexpr std::string_view Vulkan   = std::string_view("vulkan");
} // namespace api

auto GetSupportedRenderApiByPlatform(std::string_view targetApi) -> std::string_view;

class DiligentEngine
{
    public:
        DiligentEngine(std::string_view renderApi, window::NcWindow& window);
        ~DiligentEngine() noexcept;

        auto Device()    -> Diligent::IRenderDevice*  { return m_pDevice.RawPtr(); }
        auto Context()   -> Diligent::IDeviceContext* { return m_pImmediateContext.RawPtr(); }
        auto SwapChain() -> Diligent::ISwapChain*     { return m_pSwapChain.RawPtr(); }

    private:
        Diligent::RefCntAutoPtr<Diligent::IRenderDevice>  m_pDevice;
        Diligent::RefCntAutoPtr<Diligent::IDeviceContext> m_pImmediateContext;
        Diligent::RefCntAutoPtr<Diligent::ISwapChain>     m_pSwapChain;
        std::string_view                                  m_renderApi = api::Headless;
};
} // namespace graphics
} // namespace nc
