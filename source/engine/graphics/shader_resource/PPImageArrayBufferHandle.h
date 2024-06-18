#pragma once

#include "utility/Signal.h"
#include "graphics/shader_resource/ShaderTypes.h"

#include "ncutility/NcError.h"

#include <cstdint>

namespace nc::graphics
{
enum class PostProcessImageType : uint8_t
{
    None,
    ShadowMap
};

enum class PpiaUpdateAction : uint8_t
{
    Initialize,
    Update,
    Clear
};

struct PpiaUpdateEventData
{
    PostProcessImageType imageType;
    uint32_t currentFrameIndex;
    uint32_t slot;
    uint32_t set;
    uint32_t count;
    shader_stage stage;
    PpiaUpdateAction action;
};

class PPImageArrayBufferHandle
{
    public:
        PPImageArrayBufferHandle(PostProcessImageType imageType, shader_stage stage, Signal<const PpiaUpdateEventData&>* backendPort, uint32_t slot, uint32_t set = 0u);
        void Update(uint32_t currentFrameIndex);
        void Clear();

    private:
        PostProcessImageType m_imageType;
        uint32_t m_slot;
        uint32_t m_set;
        shader_stage m_stage;
        Signal<const PpiaUpdateEventData&>* m_backendPort;
};
} // namespace nc::graphics
