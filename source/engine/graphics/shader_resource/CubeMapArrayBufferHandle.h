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
    Update,
    Remove,
    Clear
};

enum class CubeMapFormat : uint8_t
{
    None,
    R8G8B8A8_SRGB,
    R32_SFLOAT
};

enum class CubeMapUsage : uint8_t
{
    None            = 0,
    TransferDst     = 1 << 0, // Can be the destination of a transfer command. (Use if the cubemap is read from a file)
    ColorAttachment = 1 << 1, // Can be used as a color attachment in a renderpass
    Sampled         = 1 << 2  // Can be sampled in a shader. 
};

inline CubeMapUsage operator | (CubeMapUsage lhs, CubeMapUsage rhs)
{
    return static_cast<CubeMapUsage>(
        static_cast<std::underlying_type_t<CubeMapUsage>>(lhs) |
        static_cast<std::underlying_type_t<CubeMapUsage>>(rhs));
};

inline CubeMapUsage operator & (CubeMapUsage lhs, CubeMapUsage rhs)
{
    return static_cast<CubeMapUsage>(
        static_cast<std::underlying_type_t<CubeMapUsage>>(lhs) &
        static_cast<std::underlying_type_t<CubeMapUsage>>(rhs));
}

struct CabUpdateEventData
{
    std::span<const asset::CubeMapWithId> data;
    uint32_t currentFrameIndex;
    uint32_t uid;
    uint32_t slot;
    uint32_t set;
    uint32_t capacity;
    shader_stage stage;
    CabUpdateAction action;
    CubeMapFormat format;
    CubeMapUsage usage;
    bool isStatic;
};

class CubeMapArrayBufferHandle
{
    public:
        CubeMapArrayBufferHandle(uint32_t uid, shader_stage stage, Signal<const CabUpdateEventData&>* backendPort, uint32_t slot, uint32_t set = 0u);
        void Add(std::span<const asset::CubeMapWithId> data, CubeMapFormat format, CubeMapUsage usage, uint32_t frameIndex);
        void Update(uint32_t currentFrameIndex);
        void Remove(std::span<const asset::CubeMapWithId> data, uint32_t frameIndex);
        void Clear(uint32_t currentFrameIndex = std::numeric_limits<uint32_t>::max());
        auto Uid() const noexcept -> uint32_t { return m_uid; }

    private:
        uint32_t m_uid;
        uint32_t m_slot;
        uint32_t m_set;
        shader_stage m_stage;
        Signal<const CabUpdateEventData&>* m_backendPort;
};
} // namespace nc::graphics
