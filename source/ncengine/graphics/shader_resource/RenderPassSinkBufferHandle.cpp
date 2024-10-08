#include "RenderPassSinkBufferHandle.h"
#include "graphics/GraphicsConstants.h"

namespace nc::graphics
{
RenderPassSinkBufferHandle::RenderPassSinkBufferHandle(RenderPassSinkType sinkType, shader_stage stage, Signal<const RpsUpdateEventData&>* backendPort, uint32_t slot, uint32_t set)
    : m_sinkType{sinkType},
      m_slot{slot},
      m_set{set},
      m_stage{stage},
      m_backendPort{backendPort}
{
    NC_ASSERT(slot < MaxResourceSlotsPerShader, "Binding slot exceeds the maximum allowed resource bindings.");
}

void RenderPassSinkBufferHandle::Update(uint32_t currentFrameIndex)
{
    m_backendPort->Emit(
        RpsUpdateEventData
        {
            m_sinkType,
            currentFrameIndex,
            m_slot,
            m_set,
            0u,
            m_stage,
            RpsUpdateAction::Update
        }
    );
}

void RenderPassSinkBufferHandle::Clear()
{
    m_backendPort->Emit(
        RpsUpdateEventData
        {
            m_sinkType,
            0u,
            m_slot,
            m_set,
            0u,
            m_stage,
            RpsUpdateAction::Clear
        }
    );
}
} // namespace nc::graphics
