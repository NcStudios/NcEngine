#include "UniformBufferHandle.h"
#include "GraphicsConstants.h"

#include "ncutility/NcError.h"

namespace nc::graphics
{
UniformBufferHandle::UniformBufferHandle(uint32_t uid, size_t size, shader_stage stage, Signal<const UboUpdateEventData&>& backendPort, uint32_t slot, uint32_t set)
    : m_uid{uid},
      m_slot{slot},
      m_set{set},
      m_size{size},
      m_stage{stage},
      m_backendPort{std::move(backendPort)}
{
    NC_ASSERT(bindingSlot < MaxResourceSlotsPerShader, "Binding slot exceeds the maximum allowed resource bindings.");
}

void UniformBufferHandle::Update(void* data, uint32_t currentFrameIndex)
{
    m_backendPort.Emit(
        UboUpdateEventData
        {
            m_uid,
            currentFrameIndex,
            m_slot,
            m_set,
            data,
            m_size,
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
            m_set,
            nullptr,
            m_size,
            m_stage,
            UboUpdateAction::Clear
        }
    );
}
} // namespace nc::graphics
