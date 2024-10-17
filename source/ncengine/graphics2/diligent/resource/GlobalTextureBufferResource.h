#pragma once

#include "Common/interface/RefCntAutoPtr.hpp"
#include "Graphics/GraphicsEngine/interface/RenderDevice.h"
#include "Graphics/GraphicsEngine/interface/DeviceContext.h"

#include <span>
#include <vector>

namespace nc
{
namespace asset
{
struct TextureWithId;
} // namespace asset

namespace graphics
{
class GlobalTextureBufferResource
{
    public:
        explicit GlobalTextureBufferResource(Diligent::IShaderResourceVariable& variable, uint32_t maxTextures)
            : m_variable{&variable},
              m_maxTextures{maxTextures}
        {
        }

        static auto MakeResourceDesc(std::string_view variableName, uint32_t maxTextures) -> Diligent::PipelineResourceDesc;
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

    private:
        std::vector<Diligent::RefCntAutoPtr<Diligent::ITexture>> m_textures;
        std::vector<Diligent::IDeviceObject*> m_objects;
        Diligent::IShaderResourceVariable* m_variable;
        uint32_t m_maxTextures;

        void SetArrayRegion(size_t offset, size_t count);
};
} // namespace nc
} // namespace graphics
