#pragma once

#include "ncutility/SpscQueue.h"

#include <limits>
#include <vector>

namespace nc::audio
{
struct BufferSlice
{
    static constexpr auto NullIndex = std::numeric_limits<uint32_t>::max();

    double* data = nullptr;
    uint32_t index = NullIndex;
};

class AudioBuffer
{
    public:
        static constexpr auto OutputChannelCount = 2u;
        static constexpr auto SampleRate = 44100u;
        static constexpr auto BufferSlices = 3u;

        explicit AudioBuffer(uint32_t bufferFrames)
            : m_memory(GetTotalBufferFrames(bufferFrames), 0.0),
              m_bufferFrames{bufferFrames}
        {
            for (auto i = 0u; i < BufferSlices; ++i)
            {
                (void)m_staleIndices.push(i);
            }
        }

        auto FramesPerBuffer()                    const -> uint32_t    { return m_bufferFrames; }
        auto SamplesPerBuffer()                   const -> uint32_t    { return FramesPerBuffer() * OutputChannelCount; }
        auto BytesPerBuffer()                     const -> uint32_t    { return SamplesPerBuffer() * sizeof(double); }
        auto AcquireStaleBuffer()                       -> BufferSlice { return AcquireBuffer(m_staleIndices); }
        auto AcquireReadyBuffer()                       -> BufferSlice { return AcquireBuffer(m_readyIndices); }
        void MarkBufferReady(const BufferSlice& buffer)                { (void)m_readyIndices.push(buffer.index); }
        void MarkBufferStale(const BufferSlice& buffer)                { (void)m_staleIndices.push(buffer.index); }

        void Resize(uint32_t bufferFrames)
        {
            Clear();
            m_memory = std::vector<double>(GetTotalBufferFrames(bufferFrames), 0.0);
            m_bufferFrames = bufferFrames;
        }

        void Clear() noexcept
        {
            auto index = 0u;
            while (m_readyIndices.pop(index))
            {
                (void)m_staleIndices.push(index);
            }
        }

    private:
        std::vector<double> m_memory;
        spsc_queue<uint32_t, BufferSlices> m_readyIndices;
        spsc_queue<uint32_t, BufferSlices> m_staleIndices;
        uint32_t m_bufferFrames;

        static auto GetTotalBufferFrames(uint32_t bufferFrames) -> uint32_t
        {
            return bufferFrames * OutputChannelCount * BufferSlices;
        }

        auto ToBufferPtr(uint32_t index) noexcept -> double*
        {
            return m_memory.data() + index * SamplesPerBuffer();
        }

        auto AcquireBuffer(spsc_queue<uint32_t, BufferSlices>& indices) -> BufferSlice
        {
            auto out = BufferSlice{};
            if (indices.pop(out.index))
            {
                out.data = ToBufferPtr(out.index);
                return out;
            }

            return out;
        }
};
} // namespace nc::audio
