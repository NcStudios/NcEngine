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
      m_backendPort{std::move(backendPort)}
{
    NC_ASSERT(slot < MaxResourceSlotsPerShader, "Binding slot exceeds the maximum allowed resource bindings.");
}

void TextureArrayBufferHandle::Update(std::span<const asset::TextureWithId> data)
{
    m_backendPort->Emit(
        TabUpdateEventData
        {
            data,
            m_uid,
            m_slot,
            m_set,
            0u,
            m_stage,
            TabUpdateAction::Update
        }
    );
}

void TextureArrayBufferHandle::Clear()
{
    m_backendPort->Emit(
        TabUpdateEventData
        {
            std::span<const asset::TextureWithId>{},
            m_uid,
            m_slot,
            m_set,
            0u,
            m_stage,
            TabUpdateAction::Clear
        }
    );
}
} // namespace nc::graphics
