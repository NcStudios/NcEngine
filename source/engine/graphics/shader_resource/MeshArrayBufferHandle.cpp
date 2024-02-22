#include "MeshArrayBufferHandle.h"
#include "graphics/GraphicsConstants.h"

#include "ncutility/NcError.h"

namespace nc::graphics
{
MeshArrayBufferHandle::MeshArrayBufferHandle(Signal<const MabUpdateEventData&>* backendPort)
    : m_backendPort{backendPort}
{
}

void MeshArrayBufferHandle::Initialize(std::span<const asset::MeshVertex> vertices, std::span<const uint32_t> indices)
{
    m_backendPort->Emit(
        MabUpdateEventData
        {
            std::numeric_limits<uint32_t>::max(),
            vertices,
            indices,
            MabUpdateAction::Initialize
        }
    );
}


void MeshArrayBufferHandle::Bind(uint32_t frameIndex)
{
    m_backendPort->Emit(
        MabUpdateEventData
        {
            frameIndex,
            std::span<const asset::MeshVertex>{},
            std::span<const uint32_t>{},
            MabUpdateAction::Bind
        }
    );
}
} // namespace nc::graphics
