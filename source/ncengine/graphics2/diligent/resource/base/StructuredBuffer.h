#pragma once
#include "graphics2/diligent/resource/ResourceTypes.h"
#include "graphics2/ShaderTypes.h"
#include "ncutility/NcError.h"

#include "Common/interface/RefCntAutoPtr.hpp"
#include "Graphics/GraphicsEngine/interface/RenderDevice.h"
#include "Graphics/GraphicsEngine/interface/DeviceContext.h"

#include <concepts>
#include <span>

namespace nc::graphics
{
class StructuredBufferBase
{
    public:
        explicit StructuredBufferBase(std::string_view name,
                                      Diligent::IShaderResourceVariable& variable,
                                      uint32_t maxElementCount,
                                      uint32_t initialElementCount);

        void Transition(Diligent::IDeviceContext& context, Diligent::RESOURCE_STATE state);

        auto GetShaderVariable()        -> Diligent::IShaderResourceVariable& { return *m_variable;       }
        auto GetElementCount()    const -> uint32_t                           { return m_elementCount;    }
        auto GetMaxElementCount() const -> uint32_t                           { return m_maxElementCount; }

    protected:
        Diligent::IShaderResourceVariable* m_variable;
        Diligent::RefCntAutoPtr<Diligent::IBuffer> m_buffer;
        std::string m_name;
        uint32_t m_maxElementCount = 0;
        uint32_t m_elementCount = 0;

        ~StructuredBufferBase() = default;

        void Reallocate(Diligent::IRenderDevice& device,
                        uint32_t elementCount,
                        uint32_t elementStride,
                        Diligent::BufferData* bufferData = nullptr);

        void SetVariable();
};

template<TriviallyCopyable T>
class StructuredBuffer : public StructuredBufferBase
{
    public:
        static constexpr auto ElementStride = static_cast<uint32_t>(sizeof(T));

        explicit StructuredBuffer(Diligent::IDeviceContext& context,
                                  Diligent::IRenderDevice& device,
                                  Diligent::IShaderResourceVariable& variable,
                                  StructuredBufferResourceDesc resourceDesc)
            : StructuredBufferBase{resourceDesc.resourceKey, variable, resourceDesc.maxElementCount, resourceDesc.initialElementCount}
        {
            CreateBuffer(context, device, resourceDesc.initialElementCount);
        }

        // Get the current buffer size in bytes.
        auto SizeBytes() const -> uint32_t { return GetElementCount() * ElementStride; }

        // Allocate a new uninitialized buffer.
        void CreateBuffer(Diligent::IDeviceContext& context,
                          Diligent::IRenderDevice& device,
                          uint32_t elementCount)
        {
            Reallocate(device, elementCount, ElementStride, nullptr);
            Transition(context, Diligent::RESOURCE_STATE_SHADER_RESOURCE);
            SetVariable();
        }

        // Allocate a new buffer with initial data.
        void CreateBuffer(Diligent::IDeviceContext& context,
                          Diligent::IRenderDevice& device,
                          std::span<const T> data)
        {
            const auto elementCount = static_cast<uint32_t>(data.size());
            auto bufferData = Diligent::BufferData{data.data(), elementCount * ElementStride};
            Reallocate(device, elementCount, ElementStride, &bufferData);
            Transition(context, Diligent::RESOURCE_STATE_SHADER_RESOURCE);
            SetVariable();
        }

        // Write data to the buffer. It is the caller's responsibility to ensure the buffer has sufficient capacity and
        // is in RESOURCE_STATE_COPY_DEST.
        void Write(Diligent::IDeviceContext& context,
                   std::span<const T> source,
                   uint64_t destinationOffset = 0u)
        {
            NC_ASSERT(source.size() + destinationOffset <= m_elementCount, "Buffer write out of bounds - buffer should be reallocated with a larger size");
            context.UpdateBuffer(
                m_buffer,
                destinationOffset * ElementStride,
                source.size() * ElementStride,
                source.data(),
                Diligent::RESOURCE_STATE_TRANSITION_MODE_VERIFY
            );
        }

        // Update buffer regions. Buffer resizing and state transitions are handled automatically.
        void Update(Diligent::IDeviceContext& context,
                    Diligent::IRenderDevice& device,
                    const BufferUpdateInfo<T>& updateInfo)
        {
            if (updateInfo.instances.size() > GetElementCount())
            {
                CreateBuffer(context, device, updateInfo.instances);
            }
            else
            {
                Transition(context, Diligent::RESOURCE_STATE_COPY_DEST);
                for (const auto& [offset, count] : updateInfo.dirtyRanges)
                {
                    const auto sourceRegion = std::span{&updateInfo.instances[offset], count};
                    Write(context, sourceRegion, offset);
                }

                Transition(context, Diligent::RESOURCE_STATE_SHADER_RESOURCE);
            }
        }
};
} // namespace nc::graphics
