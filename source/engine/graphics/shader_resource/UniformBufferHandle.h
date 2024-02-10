#pragma once

#include "utility/Signal.h"
#include "ShaderTypes.h"

#include "ncutility/NcError.h"

#include <cstdint>
#include <string>

namespace nc::graphics
{
enum class UboUpdateAction : uint8_t
{
    Initialize,
    Update,
    Clear
};

struct UboUpdateEventData
{
    std::string uid;
    uint32_t currentFrameIndex;
    uint32_t slot;
    const char* data;
    shader_stage stage;
    UboUpdateAction action;
};

class UniformBufferHandle
{
    public:
        UniformBufferHandle(std::string uid, uint32_t slot, shader_stage stage, Signal<const UboUpdateEventData&>& backendPort);
        void Update(const char* data, uint32_t currentFrameIndex);
        void Clear();

    private:
        std::string m_uid;
        uint32_t m_slot;
        shader_stage m_stage;
        Signal<const UboUpdateEventData&> m_backendPort;
};

UniformBufferHandle::UniformBufferHandle(std::string uid, uint32_t slot, shader_stage stage, Signal<const UboUpdateEventData&>& backendPort)
    : m_uid{uid},
      m_slot{slot},
      m_stage{stage},
      m_backendPort{std::move(backendPort)}
{
}

void UniformBufferHandle::Update(const char* data, uint32_t currentFrameIndex)
{
    m_backendPort.Emit(
        UboUpdateEventData
        {
            m_uid,
            currentFrameIndex,
            m_slot,
            data,
            m_stage,
            UboUpdateAction::Update
        }
    );
}

void UniformBufferHandle::Clear()
{
    m_backendPort.Emit(
        UboUpdateEventData
        {
            m_uid,
            0,
            m_slot,
            nullptr,
            m_stage,
            UboUpdateAction::Clear
        }
    );
}
}