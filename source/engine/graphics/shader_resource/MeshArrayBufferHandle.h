#pragma once

#include "utility/Signal.h"
#include "asset/AssetData.h"
#include "graphics/shader_resource/ShaderTypes.h"

#include "ncasset/Assets.h"
#include "ncutility/NcError.h"

#include <cstdint>
#include <span>

namespace nc::graphics
{
enum class MabUpdateAction : uint8_t
{
    Initialize,
    Bind
};

struct MabUpdateEventData
{
    uint32_t frameIndex;
    std::span<const asset::MeshVertex> vertices;
    std::span<const uint32_t> indices;
    MabUpdateAction action;
};

class MeshArrayBufferHandle
{
    public:
        MeshArrayBufferHandle(Signal<const MabUpdateEventData&>* backendPort);
        void Initialize(std::span<const asset::MeshVertex> vertices, std::span<const uint32_t> indices);
        void Bind(uint32_t frameIndex);

    private:
        Signal<const MabUpdateEventData&>* m_backendPort;
};
} // namespace nc::graphics
