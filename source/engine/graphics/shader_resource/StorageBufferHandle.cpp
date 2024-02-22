#include "StorageBufferHandle.h"
#include "graphics/GraphicsConstants.h"

#include "ncutility/NcError.h"

#include "optick.h"

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

void StorageBufferHandle::Bind(void* data, size_t size, uint32_t currentFrameIndex)
{
    OPTICK_CATEGORY("StorageBufferHandle::Bind", Optick::Category::Rendering);

    NC_ASSERT(size <= m_size, "Cannot bind more data to the buffer than the buffer was allocated with.");
    m_backendPort->Emit(
        SsboUpdateEventData
        {
            m_uid,
            currentFrameIndex,
            m_slot,
            m_set,
            data,
            size,
            m_stage,
            SsboUpdateAction::Update,
            false
        }
    );
}

void StorageBufferHandle::Bind(void* data, size_t size)
{
    NC_ASSERT(size <= m_size, "Cannot bind more data to the buffer than the buffer was allocated with.");
    m_backendPort->Emit(
        SsboUpdateEventData
        {
            m_uid,
            std::numeric_limits<uint32_t>::max(),
            m_slot,
            m_set,
            data,
            size,
            m_stage,
            SsboUpdateAction::Update,
            true
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
            SsboUpdateAction::Clear,
            false
        }
    );
}
} // namespace nc::graphics
