#pragma once

#include "utility/Signal.h"
#include "ShaderTypes.h"

#include "ncutility/NcError.h"

#include <cstdint>
#include <string>

namespace nc::graphics
{
enum class SsboUpdateAction : uint8_t
{
    Initialize,
    Update,
    Clear
};

// struct FooAddEvent{};
// struct FooRemoveEvent{};
// using FooEvent = std::variant<FooAddEvent, FooRemoveEvent>;

// // handler
// void HandleEvent(const FooEvent& e)
// {
//     std::visit([](auto&& unpacked)
//     {
//         using t = std::decay_t<decltype(unpacked)>;
//         if constexpr(std::same_as<t, FooAddEvent>)
//         {

//         }
//         else if const
//     }, e);
// }

struct SsboUpdateEventData
{
    uint32_t uid;
    uint32_t currentFrameIndex;
    uint32_t slot;
    uint32_t set;
    void* data;
    size_t size;
    shader_stage stage;
    SsboUpdateAction action;
    bool isStatic;
};

class StorageBufferHandle
{
    public:
        StorageBufferHandle(uint32_t uid, size_t size, shader_stage stage, Signal<const SsboUpdateEventData&>* backendPort, uint32_t slot, uint32_t set = 0u);
        void Bind(void* data, size_t size, uint32_t currentFrameIndex);
        void Bind(void* data, size_t size);
        void Clear();

    private:
        uint32_t m_uid;
        uint32_t m_slot;
        uint32_t m_set;
        size_t m_size;
        shader_stage m_stage;
        Signal<const SsboUpdateEventData&>* m_backendPort;
};
} // namespace nc::graphics
