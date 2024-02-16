#include "StorageBufferHandle.h"
#include "graphics/GraphicsConstants.h"

#include "ncutility/NcError.h"

namespace nc::graphics
{
StorageBufferHandle::StorageBufferHandle(uint32_t uid, size_t size, shader_stage stage, Signal<const SsboUpdateEventData&>* backendPort, uint32_t slot, uint32_t set)
    : m_uid{uid},
      m_slot{slot},
      m_set{set},
      m_size{size},
      m_stage{stage},
      m_backendPort{backendPort}
{
    NC_ASSERT(slot < MaxResourceSlotsPerShader, "Binding slot exceeds the maximum allowed resource bindings.");
}

void StorageBufferHandle::Update(void* data, uint32_t currentFrameIndex)
{
    m_backendPort->Emit(
        SsboUpdateEventData
        {
            m_uid,
            currentFrameIndex,
            m_slot,
            m_set,
            data,
            m_size,
            m_stage,
            SsboUpdateAction::Update
        }
    );
}

void StorageBufferHandle::Clear()
{
    m_backendPort->Emit(
        SsboUpdateEventData
        {
            m_uid,
            0,
            m_slot,
            m_set,
            nullptr,
            m_size,
            m_stage,
            SsboUpdateAction::Clear
        }
    );
}
} // namespace nc::graphics
