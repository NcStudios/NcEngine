#include "PPImageArrayBufferHandle.h"
#include "graphics/GraphicsConstants.h"

namespace nc::graphics
{
PPImageArrayBufferHandle::PPImageArrayBufferHandle(PostProcessImageType imageType, shader_stage stage, Signal<const PpiaUpdateEventData&>* backendPort, uint32_t slot, uint32_t set)
    : m_imageType{imageType},
      m_slot{slot},
      m_set{set},
      m_stage{stage},
      m_backendPort{backendPort}
{
    NC_ASSERT(slot < MaxResourceSlotsPerShader, "Binding slot exceeds the maximum allowed resource bindings.");
}

void PPImageArrayBufferHandle::Update(uint32_t count, uint32_t currentFrameIndex)
{
    m_backendPort->Emit(
        PpiaUpdateEventData
        {
            m_imageType,
            currentFrameIndex,
            m_slot,
            m_set,
            count,
            m_stage,
            PpiaUpdateAction::Update
        }
    );
}

void PPImageArrayBufferHandle::Clear()
{
    m_backendPort->Emit(
        PpiaUpdateEventData
        {
            m_imageType,
            0u,
            m_slot,
            m_set,
            0u,
            m_stage,
            PpiaUpdateAction::Clear
        }
    );
}
} // namespace nc::graphics
