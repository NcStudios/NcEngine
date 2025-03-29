#pragma once
#include "ncengine/asset/AssetData.h"

#include "Common/interface/RefCntAutoPtr.hpp"
#include "Graphics/GraphicsEngine/interface/RenderDevice.h"
#include "Graphics/GraphicsEngine/interface/DeviceContext.h"

#include <span>
#include <vector>

namespace nc
{
namespace graphics
{
class CubeMapBufferResource
{
    public:
        explicit CubeMapBufferResource(Diligent::IShaderResourceVariable& variable, uint32_t maxCubeMaps);
        static auto MakeSamplerDesc(std::string_view variableName) -> Diligent::ImmutableSamplerDesc;

        void Load(std::span<const asset::CubeMapWithId> cubeMaps,
                  Diligent::IDeviceContext& context,
                  Diligent::IRenderDevice& device);

        void Unload();

        auto GetShaderVariable() -> Diligent::IShaderResourceVariable&
        {
            return *m_variable;
        }

        auto GetMaxCubeMapCount() const -> size_t
        {
            return m_maxCubeMaps;
        }

    private:
        std::vector<Diligent::RefCntAutoPtr<Diligent::ITexture>> m_cubeMaps;
        std::vector<Diligent::IDeviceObject*> m_shaderResourceViews;
        std::vector<Diligent::IDeviceObject*> m_renderTargetViews;
        Diligent::IShaderResourceVariable* m_variable;
        uint32_t m_maxCubeMaps;
        bool m_initialLoadComplete;
};
} // namespace nc
} // namespace graphics
