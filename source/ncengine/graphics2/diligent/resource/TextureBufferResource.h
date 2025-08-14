#pragma once
#include "ncengine/asset/AssetData.h"
#include "ncutility/NcError.h"

#include "Common/interface/RefCntAutoPtr.hpp"
#include "Graphics/GraphicsEngine/interface/RenderDevice.h"
#include "Graphics/GraphicsEngine/interface/DeviceContext.h"

#include <span>
#include <vector>

namespace nc
{
namespace graphics
{
class TextureBufferResource
{
    public:
        explicit TextureBufferResource(Diligent::IShaderResourceVariable& variable, uint32_t maxTextures);
        static auto MakeSamplerDesc(std::string_view variableName) -> Diligent::ImmutableSamplerDesc;

        void Load(std::span<const asset::TextureWithId> textures,
                  Diligent::IDeviceContext& context,
                  Diligent::IRenderDevice& device);

        void Unload();

        auto GetShaderVariable() -> Diligent::IShaderResourceVariable&
        {
            return *m_variable;
        }

        auto GetMaxTextureCount() const -> size_t
        {
            return m_maxTextures;
        }

        auto GetTextureView(uint32_t index) -> void*
        {
            NC_ASSERT(index < m_views.size(), "Invalid texture view index.");
            return static_cast<void*>(m_views.at(index));
        }

    private:
        std::vector<Diligent::RefCntAutoPtr<Diligent::ITexture>> m_textures;
        std::vector<Diligent::IDeviceObject*> m_views;
        Diligent::IShaderResourceVariable* m_variable;
        uint32_t m_maxTextures;
        bool m_initialLoadComplete;
};
} // namespace nc
} // namespace graphics
