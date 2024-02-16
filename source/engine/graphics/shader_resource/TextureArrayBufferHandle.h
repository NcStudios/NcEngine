#pragma once

#include "utility/Signal.h"
#include "asset/AssetData.h"
#include "ShaderTypes.h"

#include "ncasset/Assets.h"
#include "ncutility/NcError.h"

#include <cstdint>
#include <span>
#include <string>

namespace nc::graphics
{
enum class TabUpdateAction : uint8_t
{
    Initialize,
    Update,
    Clear
};

struct TabUpdateEventData
{
    std::span<const asset::TextureWithId> data;
    uint32_t uid;
    uint32_t slot;
    uint32_t set;
    uint32_t capacity;
    shader_stage stage;
    TabUpdateAction action;
};

class TextureArrayBufferHandle
{
    public:
        TextureArrayBufferHandle(uint32_t uid, shader_stage stage, Signal<const TabUpdateEventData&>* backendPort, uint32_t slot, uint32_t set = 0u);
        void Update(std::span<const asset::TextureWithId> data);
        void Clear();

    private:
        uint32_t m_uid;
        uint32_t m_slot;
        uint32_t m_set;
        shader_stage m_stage;
        Signal<const TabUpdateEventData&>* m_backendPort;
};
} // namespace nc::graphics
