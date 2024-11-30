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
        template<TriviallyCopyable T>
        explicit UniformBuffer(Diligent::IDeviceContext& context,
                               Diligent::IRenderDevice& device,
                               const T& initialData,
                               std::string_view name = "UniformBuffer")
            : UniformBuffer{
                context,
                device,
                static_cast<const void*>(&initialData),
                sizeof(T),
                name
              }
        {
        }

        auto GetBuffer()     -> Diligent::IBuffer& { return *m_buffer; }
        auto GetSize() const -> size_t             { return m_size; }

        // Update the buffer contents with a source object.
        template<TriviallyCopyable T>
        void Write(Diligent::IDeviceContext& context, const T& source)
        {
            Write(context, static_cast<const void*>(&source), sizeof(source));
        }

        // Get a pointer to a mapped region for writing.
        auto Map(Diligent::IDeviceContext& context) -> void*;

        // Unmap the buffer.
        void Unmap(Diligent::IDeviceContext& context);

    private:
        Diligent::RefCntAutoPtr<Diligent::IBuffer> m_buffer;
        size_t m_size;

        UniformBuffer(Diligent::IDeviceContext& context,
                      Diligent::IRenderDevice& device,
                      const void* data,
                      size_t size,
                      std::string_view name);

        void Write(Diligent::IDeviceContext& context, const void* src, size_t size);
};
} // namespace nc::graphics
