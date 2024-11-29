#pragma once

#include "graphics2/ShaderTypes.h"

#include "Common/interface/RefCntAutoPtr.hpp"
#include "Graphics/GraphicsEngine/interface/RenderDevice.h"
#include "Graphics/GraphicsEngine/interface/DeviceContext.h"

#include <span>
#include <string_view>

namespace nc::graphics
{
class UniformBuffer
{
    public:
        explicit UniformBuffer(Diligent::IDeviceContext& context,
                               Diligent::IRenderDevice& device,
                               size_t size,
                               std::string_view name = "UniformBuffer");

        template<TriviallyCopyable T>
        void Update(Diligent::IDeviceContext& context, const T& source)
        {
            Update(context, static_cast<const void*>(&source), sizeof(source));
        }

        // could maybe implement read function for testing...

        auto GetBuffer() -> Diligent::IBuffer&
        {
            return *m_buffer;
        }

    private:
        Diligent::RefCntAutoPtr<Diligent::IBuffer> m_buffer;
        size_t m_size;

        void Update(Diligent::IDeviceContext& context, const void* src, size_t size);
};
} // namespace nc::graphics
