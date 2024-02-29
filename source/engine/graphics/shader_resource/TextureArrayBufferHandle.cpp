#include "TextureArrayBufferHandle.h"
#include "graphics/GraphicsConstants.h"

#include "optick.h"

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

void TextureArrayBufferHandle::Add(std::span<const asset::TextureWithId> data)
{
    OPTICK_CATEGORY("TextureArrayBufferHandle::Add", Optick::Category::Rendering);

    m_backendPort->Emit(
        TabUpdateEventData
        {
            data,
            m_uid,
            m_slot,
            m_set,
            0u,
            m_stage,
            TabUpdateAction::Add
        }
    );
}

void TextureArrayBufferHandle::Remove(std::span<const asset::TextureWithId> data)
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
            TabUpdateAction::Remove
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
