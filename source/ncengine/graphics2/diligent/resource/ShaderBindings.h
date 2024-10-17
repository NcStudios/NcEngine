#pragma once

#include "GlobalResourceSignature.h"

#include "Graphics/GraphicsEngine/interface/RenderDevice.h"

namespace nc::graphics
{
class ShaderBindings
{
    public:
        explicit ShaderBindings(Diligent::IRenderDevice& device, uint32_t maxTextures)
            : m_globalSignature{device, maxTextures}
        {
        }

        auto GetGlobalSignature() -> GlobalResourceSignature&
        {
            return m_globalSignature;
        }

    private:
        GlobalResourceSignature m_globalSignature;
};
} // namespace nc::graphics
