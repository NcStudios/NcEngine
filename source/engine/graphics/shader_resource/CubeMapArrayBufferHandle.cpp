#include "CubeMapArrayBufferHandle.h"
#include "graphics/GraphicsConstants.h"

namespace nc::graphics
{
CubeMapArrayBufferHandle::CubeMapArrayBufferHandle(uint32_t uid, shader_stage stage, Signal<const CabUpdateEventData&>* backendPort, uint32_t slot, uint32_t set)
    : m_uid{uid},
      m_slot{slot},
      m_set{set},
      m_stage{stage},
      m_backendPort{backendPort}
{
    NC_ASSERT(slot < MaxResourceSlotsPerShader, "Binding slot exceeds the maximum allowed resource bindings.");
}

void CubeMapArrayBufferHandle::Add(std::span<const asset::CubeMapWithId> data)
{
    m_backendPort->Emit(
        CabUpdateEventData
        {
            data,
            m_uid,
            m_slot,
            m_set,
            0u,
            m_stage,
            CabUpdateAction::Add
        }
    );
}

void CubeMapArrayBufferHandle::Remove(std::span<const asset::CubeMapWithId> data)
{
    m_backendPort->Emit(
        CabUpdateEventData
        {
            data,
            m_uid,
            m_slot,
            m_set,
            0u,
            m_stage,
            CabUpdateAction::Remove
        }
    );
}

void CubeMapArrayBufferHandle::Clear()
{
    m_backendPort->Emit(
        CabUpdateEventData
        {
            std::span<const asset::CubeMapWithId>{},
            m_uid,
            m_slot,
            m_set,
            0u,
            m_stage,
            CabUpdateAction::Clear
        }
    );
}
} // namespace nc::graphics
