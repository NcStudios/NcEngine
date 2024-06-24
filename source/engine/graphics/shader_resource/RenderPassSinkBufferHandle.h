#pragma once

#include "utility/Signal.h"
#include "graphics/shader_resource/ShaderTypes.h"

#include "ncutility/NcError.h"

#include <cstdint>

namespace nc::graphics
{
enum class RenderPassSinkType : uint8_t
{
    None,
    UniDirShadowMap,
    OmniDirShadowMap
};

enum class RpsUpdateAction : uint8_t
{
    Initialize,
    Update,
    Clear
};

struct RpsUpdateEventData
{
    RenderPassSinkType sinkType;
    uint32_t currentFrameIndex;
    uint32_t slot;
    uint32_t set;
    uint32_t count;
    shader_stage stage;
    RpsUpdateAction action;
};

class RenderPassSinkBufferHandle
{
    public:
        RenderPassSinkBufferHandle(RenderPassSinkType sinkType, shader_stage stage, Signal<const RpsUpdateEventData&>* backendPort, uint32_t slot, uint32_t set = 0u);
        void Update(uint32_t currentFrameIndex);
        void Clear();

    private:
        RenderPassSinkType m_sinkType;
        uint32_t m_slot;
        uint32_t m_set;
        shader_stage m_stage;
        Signal<const RpsUpdateEventData&>* m_backendPort;
};
} // namespace nc::graphics
