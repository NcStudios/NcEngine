#pragma once
#include "graphics2/ShaderTypes.h"
#include "ncutility/NcError.h"

#include "Common/interface/RefCntAutoPtr.hpp"
#include "Graphics/GraphicsEngine/interface/RenderDevice.h"
#include "Graphics/GraphicsEngine/interface/DeviceContext.h"

#include <concepts>
#include <span>
#include <vector>

namespace nc::graphics
{
template<TriviallyCopyable T>
class StructuredBuffer
{
    public:
        explicit StructuredBuffer(Diligent::IDeviceContext& context,
                                  Diligent::IRenderDevice& device,
                                  std::string_view name,
                                  Diligent::IShaderResourceVariable& variable,
                                  uint32_t initialSize);

        static auto MakeResourceDesc(std::string_view variableName, Diligent::SHADER_TYPE shaderType) -> Diligent::PipelineResourceDesc;
        auto GetShaderVariable() -> Diligent::IShaderResourceVariable& { return *m_variable; }
        void Update(Diligent::IDeviceContext& context,
                    Diligent::IRenderDevice& device,
                    const BufferUpdateInfo<T>& udpateInfo);

    private:
        void CreateBuffer(Diligent::IDeviceContext& context,
                          Diligent::IRenderDevice& device,
                          std::span<const T> data);

        void Transition(Diligent::IDeviceContext& context,
                        Diligent::RESOURCE_STATE state);

        Diligent::IShaderResourceVariable* m_variable;
        Diligent::RefCntAutoPtr<Diligent::IBuffer> m_buffer;
        std::string_view m_name;
        uint32_t m_bufferElementCount = 0;
};

template<TriviallyCopyable T>
StructuredBuffer<T>::StructuredBuffer(Diligent::IDeviceContext& context,
                                      Diligent::IRenderDevice& device,
                                      std::string_view name,
                                      Diligent::IShaderResourceVariable& variable,
                                      uint32_t initialSize)
: m_variable{&variable},
  m_name{name}
{
    const auto dummy = std::vector<T>(initialSize);
    CreateBuffer(context, device, dummy);
}

template<TriviallyCopyable T>
auto StructuredBuffer<T>::MakeResourceDesc(std::string_view variableName,
                                           Diligent::SHADER_TYPE shaderType) -> Diligent::PipelineResourceDesc
{
    return Diligent::PipelineResourceDesc{
        shaderType,
        variableName.data(),
        1,
        Diligent::SHADER_RESOURCE_TYPE_BUFFER_SRV,
        Diligent::SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE
    };
}

template<TriviallyCopyable T>
void StructuredBuffer<T>::Transition(Diligent::IDeviceContext& context,
                                     Diligent::RESOURCE_STATE state)
{
    const auto barrier = Diligent::StateTransitionDesc(
        m_buffer,
        Diligent::RESOURCE_STATE_UNKNOWN,
        state,
        Diligent::STATE_TRANSITION_FLAG_UPDATE_STATE
    );

    context.TransitionResourceStates(1, &barrier);
}

template<TriviallyCopyable T>
void StructuredBuffer<T>::CreateBuffer(Diligent::IDeviceContext& context,
                                       Diligent::IRenderDevice& device,
                                       std::span<const T> data)
{
    constexpr auto elementSize = static_cast<uint32_t>(sizeof(T));
    m_bufferElementCount = static_cast<uint32_t>(data.size());
    const auto bufferSize = elementSize * m_bufferElementCount;
    const auto bufferDesc = Diligent::BufferDesc{
        m_name.data(),
        bufferSize,
        Diligent::BIND_SHADER_RESOURCE,
        Diligent::USAGE_DEFAULT,
        Diligent::CPU_ACCESS_NONE,
        Diligent::BUFFER_MODE_STRUCTURED,
        elementSize
    };

    auto bufferData = Diligent::BufferData{data.data(), bufferSize};
    m_buffer.Release();
    device.CreateBuffer(bufferDesc, &bufferData, &m_buffer);
    if (!m_buffer)
    {
        throw nc::NcError{fmt::format("Failed to create buffer '{}'", bufferDesc.Name)};
    }

    Transition(context, Diligent::RESOURCE_STATE_SHADER_RESOURCE);
    m_variable->Set(
        m_buffer->GetDefaultView(Diligent::BUFFER_VIEW_SHADER_RESOURCE),
        Diligent::SET_SHADER_RESOURCE_FLAG_ALLOW_OVERWRITE
    );
}

template<TriviallyCopyable T>
void StructuredBuffer<T>::Update(Diligent::IDeviceContext& context,
                                 Diligent::IRenderDevice& device,
                                 const BufferUpdateInfo<T>& updateInfo)
{
    if (updateInfo.instances.size() > m_bufferElementCount)
    {
        CreateBuffer(context, device, updateInfo.instances);
    }
    else
    {
        Transition(context, Diligent::RESOURCE_STATE_COPY_DEST);
        constexpr auto elementSize = sizeof(MaterialData);
        for (const auto& [offset, count] : updateInfo.dirtyRanges)
        {
            const auto source = &updateInfo.instances[offset];
            context.UpdateBuffer(
                m_buffer,
                offset * elementSize,
                count * elementSize,
                source,
                Diligent::RESOURCE_STATE_TRANSITION_MODE_VERIFY
            );
        }
        Transition(context, Diligent::RESOURCE_STATE_SHADER_RESOURCE);
    }
}
} // namespace nc::graphics