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

void CubeMapArrayBufferHandle::Add(std::span<const asset::CubeMapWithId> data, CubeMapFormat format, CubeMapUsage usage, uint32_t frameIndex)
{
    m_backendPort->Emit(
        CabUpdateEventData
        {
            data,
            frameIndex,
            m_uid,
            m_slot,
            m_set,
            0u,
            m_stage,
            CabUpdateAction::Add,
            format,
            usage,
            frameIndex == std::numeric_limits<uint32_t>::max() ? true : false
        }
    );
}

void CubeMapArrayBufferHandle::Update(uint32_t frameIndex)
{
    m_backendPort->Emit(
        CabUpdateEventData
        {
            std::span<const asset::CubeMapWithId>{},
            frameIndex,
            m_uid,
            m_slot,
            m_set,
            0u,
            m_stage,
            CabUpdateAction::Update,
            CubeMapFormat::None,
            CubeMapUsage::None,
            frameIndex == std::numeric_limits<uint32_t>::max() ? true : false
        }
    );
}

void CubeMapArrayBufferHandle::Remove(std::span<const asset::CubeMapWithId> data, uint32_t frameIndex)
{
    m_backendPort->Emit(
        CabUpdateEventData
        {
            data,
            frameIndex,
            m_uid,
            m_slot,
            m_set,
            0u,
            m_stage,
            CabUpdateAction::Remove,
            CubeMapFormat::None,
            CubeMapUsage::None,
            frameIndex == std::numeric_limits<uint32_t>::max() ? true : false
        }
    );
}

void CubeMapArrayBufferHandle::Clear(uint32_t frameIndex)
{
    m_backendPort->Emit(
        CabUpdateEventData
        {
            std::span<const asset::CubeMapWithId>{},
            frameIndex,
            m_uid,
            m_slot,
            m_set,
            0u,
            m_stage,
            CabUpdateAction::Clear,
            CubeMapFormat::None,
            CubeMapUsage::None,
            frameIndex == std::numeric_limits<uint32_t>::max() ? true : false
        }
    );
}
} // namespace nc::graphics
