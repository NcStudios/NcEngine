#include "UniformBufferHandle.h"
#include "graphics/GraphicsConstants.h"

#include "ncutility/NcError.h"

namespace nc::graphics
{
UniformBufferHandle::UniformBufferHandle(uint32_t uid, size_t size, shader_stage stage, Signal<const UboUpdateEventData&>* backendPort, uint32_t slot, uint32_t set)
    : m_uid{uid},
      m_slot{slot},
      m_set{set},
      m_size{size},
      m_stage{stage},
      m_backendPort{backendPort}
{
    NC_ASSERT(slot < MaxResourceSlotsPerShader, "Binding slot exceeds the maximum allowed resource bindings.");
}

void UniformBufferHandle::Update(void* data, size_t size, uint32_t currentFrameIndex)
{
    NC_ASSERT(size <= m_size, "Cannot bind more data to the buffer than the buffer was allocated with.");
    m_backendPort->Emit(
        UboUpdateEventData
        {
            m_uid,
            currentFrameIndex,
            m_slot,
            m_set,
            data,
            m_size,
            m_stage,
            UboUpdateAction::Update,
            false
        }
    );
}

void UniformBufferHandle::Update(void* data, size_t size)
{
    NC_ASSERT(size <= m_size, "Cannot bind more data to the buffer than the buffer was allocated with.");
    m_backendPort->Emit(
        UboUpdateEventData
        {
            m_uid,
            std::numeric_limits<uint32_t>::max(),
            m_slot,
            m_set,
            data,
            m_size,
            m_stage,
            UboUpdateAction::Update,
            true
        }
    );
}

void UniformBufferHandle::Clear()
{
    m_backendPort->Emit(
        UboUpdateEventData
        {
            m_uid,
            0,
            m_slot,
            m_set,
            nullptr,
            m_size,
            m_stage,
            UboUpdateAction::Clear,
            false
        }
    );
}
} // namespace nc::graphics
