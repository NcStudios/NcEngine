#pragma once

#include "Common/interface/RefCntAutoPtr.hpp"
#include "Graphics/GraphicsEngine/interface/RenderDevice.h"
#include "Graphics/GraphicsEngine/interface/DeviceContext.h"

#include <vector>

namespace nc::graphics
{
class PostProcessColorSinkBufferResource
{
    public:
        explicit PostProcessColorSinkBufferResource(Diligent::IShaderResourceVariable& variable, uint32_t maxTextures)
            : m_variable{&variable},
              m_maxTextures{maxTextures}
        {
        }

        static auto MakeSamplerDesc(std::string_view variableName) -> Diligent::ImmutableSamplerDesc;

        auto Add(Diligent::IRenderDevice& device,
                 uint32_t numColorRenderTargets,
                 uint32_t renderTargetWidth,
                 uint32_t renderTargetHeight,
                 uint32_t numSamples = 1u) -> std::vector<uint32_t>;

        void Resize(Diligent::IRenderDevice& device,
                    uint32_t renderTargetWidth,
                    uint32_t renderTargetHeight,
                    uint32_t numSamples = 1u);

        void Clear();
        auto GetMsaaSrv(uint32_t index) -> Diligent::IDeviceObject* { return m_colorRenderTargetViewsRTMsaa.at(index); }
        auto GetSrv(uint32_t index) -> Diligent::IDeviceObject* { return m_colorRenderTargetViewsRT.at(index); }
        auto GetMsaaTexture(uint32_t index) -> Diligent::ITexture* { return m_colorRenderTargetsMsaa.at(index); }
        auto GetTexture(uint32_t index) -> Diligent::ITexture* { return m_colorRenderTargets.at(index); }

    private:
        std::vector<Diligent::RefCntAutoPtr<Diligent::ITexture>> m_colorRenderTargets;
        std::vector<Diligent::IDeviceObject*> m_colorRenderTargetViewsSR; // Shader resource
        std::vector<Diligent::IDeviceObject*> m_colorRenderTargetViewsRT; // Render target
        std::vector<Diligent::RefCntAutoPtr<Diligent::ITexture>> m_colorRenderTargetsMsaa;
        std::vector<Diligent::IDeviceObject*> m_colorRenderTargetViewsSRMsaa; // Shader resource
        std::vector<Diligent::IDeviceObject*> m_colorRenderTargetViewsRTMsaa; // Render target
        Diligent::IShaderResourceVariable* m_variable;
        uint32_t m_maxTextures;

        void SetArrayRegion(const std::vector<Diligent::IDeviceObject*>& combinedViews, size_t offset, size_t count);
};
} // namespace nc::graphics
