#pragma once

#include "Common/interface/RefCntAutoPtr.hpp"
#include "Graphics/GraphicsEngine/interface/RenderDevice.h"
#include "Graphics/GraphicsEngine/interface/DeviceContext.h"

#include <vector>

namespace nc::graphics
{
class PostProcessDepthSinkBufferResource
{
    public:
        explicit PostProcessDepthSinkBufferResource(Diligent::IShaderResourceVariable& variable, uint32_t maxTextures)
            : m_variable{&variable},
              m_maxTextures{maxTextures}
        {
        }

        auto Add(Diligent::IRenderDevice& device,
                 uint32_t numDepthRenderTargets,
                 uint32_t renderTargetWidth,
                 uint32_t renderTargetHeight) -> std::vector<uint32_t>;

        void Resize(Diligent::IRenderDevice& device,
                    uint32_t renderTargetWidth,
                    uint32_t renderTargetHeight);

        void Clear();
        auto GetDepthRenderTarget(uint32_t index) -> Diligent::IDeviceObject* { return m_depthRenderTargetViewsRT.at(index); }

    private:
        std::vector<Diligent::RefCntAutoPtr<Diligent::ITexture>> m_depthRenderTargets;
        std::vector<Diligent::IDeviceObject*> m_depthRenderTargetViewsSR; // Shader Resource
        std::vector<Diligent::IDeviceObject*> m_depthRenderTargetViewsRT; // Render target
        Diligent::IShaderResourceVariable* m_variable;
        uint32_t m_maxTextures;

        void SetArrayRegion(const std::vector<Diligent::IDeviceObject*>& combinedViews, size_t offset, size_t count);
};
} // namespace nc::graphics
