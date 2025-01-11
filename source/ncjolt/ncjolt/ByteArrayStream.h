#pragma once

#include "Jolt/Jolt.h"
#include "Jolt/Core/StreamIn.h"
#include "Jolt/Core/StreamOut.h"

#include <cstring>
#include <vector>

namespace nc::jolt
{
// Jolt I/O stream implementation backed by a std::vector
class ByteArrayStream : public JPH::StreamIn,
                        public JPH::StreamOut
{
    public:
        explicit ByteArrayStream() = default;

        explicit ByteArrayStream(std::vector<uint8_t> data)
            : m_data{std::move(data)},
              m_writePos{m_data.size()}
        {
        }

        void ReadBytes(void* out, size_t numBytes) override
        {
            if (m_readPos + numBytes > m_data.size())
            {
                const auto availableBytes = m_data.size() - m_readPos;
                if (availableBytes > 0)
                {
                    std::memcpy(out, m_data.data() + m_readPos, availableBytes);
                }

                std::memset(static_cast<uint8_t*>(out) + availableBytes, 0, numBytes - availableBytes);
                m_readPos = m_data.size();
                m_failed = true;
                return;
            }

            std::memcpy(out, m_data.data() + m_readPos, numBytes);
            m_readPos += numBytes;
        }

        void WriteBytes(const void* data, size_t numBytes) override
        {
            if (m_writePos + numBytes > m_data.size())
            {
                m_data.resize(m_writePos + numBytes);
            }

            std::memcpy(m_data.data() + m_writePos, data, numBytes);
            m_writePos += numBytes;
        }

        auto IsEOF() const -> bool override
        {
            return m_failed && m_readPos >= m_data.size();
        }

        auto IsFailed() const -> bool override
        {
            return m_failed;
        }

        auto GetBuffer() const -> const std::vector<uint8_t>&
        {
            return m_data;
        }

        auto ExtractBuffer() -> std::vector<uint8_t>
        {
            m_writePos = 0;
            m_readPos = 0;
            m_failed = false;
            return std::move(m_data);
        }

        void Reset() noexcept
        {
            m_data.clear();
            m_writePos = 0;
            m_readPos = 0;
            m_failed = false;
        }

    private:
        std::vector<uint8_t> m_data;
        size_t m_writePos = 0;
        size_t m_readPos = 0;
        bool m_failed = false;
};
} // namespace nc::jolt
