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
    uint32_t uid;
    uint32_t currentFrameIndex;
    uint32_t slot;
    uint32_t set;
    void* data;
    size_t size;
    shader_stage stage;
    UboUpdateAction action;
};

class UniformBufferHandle
{
    public:
        UniformBufferHandle(uint32_t uid, size_t size, shader_stage stage, Signal<const UboUpdateEventData&>* backendPort, uint32_t slot, uint32_t set = 0u);
        void Update(void* data, uint32_t currentFrameIndex);
        void Clear();

    private:
        uint32_t m_uid;
        uint32_t m_slot;
        uint32_t m_set;
        size_t m_size;
        shader_stage m_stage;
        Signal<const UboUpdateEventData&>* m_backendPort;
};
} // namespace nc::graphics
