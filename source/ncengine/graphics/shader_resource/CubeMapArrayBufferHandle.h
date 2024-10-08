#pragma once

#include "asset/AssetData.h"
#include "utility/Signal.h"
#include "graphics/shader_resource/ShaderTypes.h"

#include "ncasset/Assets.h"
#include "ncutility/NcError.h"

#include <cstdint>
#include <span>

namespace nc::graphics
{
enum class CabUpdateAction : uint8_t
{
    Initialize,
    Add,
    Remove,
    Clear
};

struct CabUpdateEventData
{
    std::span<const asset::CubeMapWithId> data;
    uint32_t uid;
    uint32_t slot;
    uint32_t set;
    uint32_t capacity;
    shader_stage stage;
    CabUpdateAction action;
};

class CubeMapArrayBufferHandle
{
    public:
        CubeMapArrayBufferHandle(uint32_t uid, shader_stage stage, Signal<const CabUpdateEventData&>* backendPort, uint32_t slot, uint32_t set = 0u);
        void Add(std::span<const asset::CubeMapWithId> data);
        void Remove(std::span<const asset::CubeMapWithId> data);
        void Clear();

    private:
        uint32_t m_uid;
        uint32_t m_slot;
        uint32_t m_set;
        shader_stage m_stage;
        Signal<const CabUpdateEventData&>* m_backendPort;
};
} // namespace nc::graphics
