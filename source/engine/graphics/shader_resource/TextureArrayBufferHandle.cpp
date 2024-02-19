#include "TextureArrayBufferHandle.h"
#include "graphics/GraphicsConstants.h"

#include "ncutility/NcError.h"

namespace nc::graphics
{
TextureArrayBufferHandle::TextureArrayBufferHandle(uint32_t uid, shader_stage stage, Signal<const TabUpdateEventData&>* backendPort, uint32_t slot, uint32_t set)
    : m_uid{uid},
      m_slot{slot},
      m_set{set},
      m_stage{stage},
      m_backendPort{backendPort}
{
    NC_ASSERT(slot < MaxResourceSlotsPerShader, "Binding slot exceeds the maximum allowed resource bindings.");
}

void TextureArrayBufferHandle::Update(std::span<const asset::TextureWithId> data, uint32_t frameIndex)
{
    m_backendPort->Emit(
        TabUpdateEventData
        {
            data,
            frameIndex,
            m_uid,
            m_slot,
            m_set,
            0u,
            m_stage,
            TabUpdateAction::Update,
            false
        }
    );
}

void TextureArrayBufferHandle::Update(std::span<const asset::TextureWithId> data)
{
    m_backendPort->Emit(
        TabUpdateEventData
        {
            data,
            std::numeric_limits<uint32_t>::max(),
            m_uid,
            m_slot,
            m_set,
            0u,
            m_stage,
            TabUpdateAction::Update,
            true
        }
    );
}

void TextureArrayBufferHandle::Clear()
{
    m_backendPort->Emit(
        TabUpdateEventData
        {
            std::span<const asset::TextureWithId>{},
            std::numeric_limits<uint32_t>::max(),
            m_uid,
            m_slot,
            m_set,
            0u,
            m_stage,
            TabUpdateAction::Clear,
            false
        }
    );
}
} // namespace nc::graphics
