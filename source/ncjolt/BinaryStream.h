#pragma once

#include "Jolt/Jolt.h"
#include "Jolt/Core/StreamIn.h"
#include "Jolt/Core/StreamOut.h"

#include <cstring>
#include <vector>

namespace nc::jolt
{
class BinaryStream : public JPH::StreamIn,
                     public JPH::StreamOut
{
    public:
        explicit BinaryStream() = default;

        explicit BinaryStream(std::vector<uint8_t> data)
            : m_data{std::move(data)},
              m_writePos{m_data.size()}
        {
        }

        void ReadBytes(void* out, size_t numBytes) override
        {
            const auto bytes = std::min(numBytes, m_data.size() - m_readPos);
            if (bytes < numBytes)
            {
                m_failed = true;
            }

            std::memcpy(out, m_data.data() + m_readPos, bytes);
            m_readPos += bytes;
        }

        void WriteBytes(const void* data, size_t numBytes) override
        {
            // check resizeing
            m_data.resize(m_data.size() + numBytes);
            std::memcpy(m_data.data() + m_writePos, data, numBytes);
            m_writePos += numBytes;
        }

        auto IsEOF() const -> bool override
        {
            return m_failed;
        }

        auto IsFailed() const -> bool override
        {
            return false;
        }

        auto Extract() -> std::vector<uint8_t>
        {
            m_writePos = 0;
            return std::move(m_data);
        }

    private:
        std::vector<uint8_t> m_data;
        size_t m_writePos = 0;
        size_t m_readPos = 0;
        bool m_failed = false;
};
} // namespace nc::jolt
