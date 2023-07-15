#pragma once

#include "ncengine/type/Type.h"

#include "ncutility/NcError.h"

#include <algorithm>
#include <array>
#include <cstring>
#include <filesystem>
#include <span>
#include <string>
#include <vector>

namespace nc::serialize
{
/**
 * @brief Read a file as a vector of bytes.
 * @param filePath The path to the file to read from.
 * @return The bytes read.
 */
auto ReadBinaryFile(const std::filesystem::path& filePath) -> std::vector<uint8_t>;

/**
 * @brief Write a span of bytes to a file.
 * @param filePath The path to the file to write to.
 * @param bytes The bytes to write.
 */
void WriteBinaryFile(const std::filesystem::path& filePath, std::span<const uint8_t> bytes);

/**
 * @brief 
 */
class BinaryBuffer
{
    public:
        /**
         * @brief Construct a new BinaryBuffer.
         * @param sizeHint The buffer's initial capacity.
        */
        explicit BinaryBuffer(size_t sizeHint = 1024)
            : m_data(sizeHint, 0) {}

        /**
         * @brief Construct a BinaryBuffer from a vector of bytes.
         * @param data The bytes to construct the BinaryBuffer with.
        */
        explicit BinaryBuffer(std::vector<uint8_t> data)
            : m_data{std::move(data)} {}

        /**
         * @brief Construct a BinaryBuffer from the contents of a file.
         * @param filePath The path to the file to read from.
        */
        explicit BinaryBuffer(const std::filesystem::path& filePath)
            : m_data{ReadBinaryFile(filePath)},
              m_writePos{m_data.size()} {}

        /**
         * @brief Read bytes from the buffer.
         * @param out Destination to write to.
         * @param nBytes The number of bytes to read.
        */
        void Read(void* out, size_t nBytes)
        {
            AssertRead(nBytes);
            std::memcpy(out, m_data.data() + m_readPos, nBytes);
            m_readPos += nBytes;
        }

        /**
         * @brief Read a trivially copyable object from the buffer.
         * @param out Destination to write to.
        */
        template<type::TriviallyCopyable T>
        void Read(T& out)
        {
            Read(static_cast<void*>(&out), sizeof(T));
        }

        /**
         * @brief Write bytes to the buffer.
         * @param in The source to read from.
         * @param nBytes The number of bytes to write.
        */
        void Write(const void* in, size_t nBytes)
        {
            EnsureCapacity(nBytes);
            std::memcpy(m_data.data() + m_writePos, in, nBytes);
            m_writePos += nBytes;
        }

        /**
         * @brief Write a trivially copyable object to the buffer.
         * @param in The source to read from.
        */
        template<type::TriviallyCopyable T>
        void Write(const T& in)
        {
            Write(static_cast<const void*>(&in), sizeof(T));
        }

        /**
         * @brief Get the size of the buffer.
         * @return The number of bytes available for reading.
        */
        auto Size() const noexcept
        {
            return m_writePos;
        }

        /**
         * @brief Get the capacity of the buffer.
         * @return The number of bytes allocated by the BinaryBuffer.
        */
        auto Capacity() const noexcept
        {
            return m_data.size();
        }

        /**
         * @brief Change the capacity of the buffer.
         * @param nBytes The new size of the BinaryBuffer.
         * @note The read and write positions will be updated if nBytes
         *       is less than Size().
        */
        void Resize(size_t nBytes)
        {
            m_data.resize(nBytes, 0);
            if (nBytes == 0)
            {
                m_readPos = 0;
                m_writePos = 0;
                return;
            }

            m_readPos = std::min(m_readPos, nBytes);
            m_writePos = std::min(m_writePos, nBytes);
        }

        /** @brief Clear the buffer and reset the read and write positions. */
        void Clear() noexcept
        {
            m_data.clear();
            m_readPos = 0;
            m_writePos = 0;
        }

        /**
         * @brief 
         * 
         */
        auto GetReadPos() const noexcept -> size_t
        {
            return m_readPos;
        }

        /**
         * @brief Set the position of the read head.
         * @param pos A byte offset.
         * @note The new read position will be clamped to >= Size().
         */
        void SetReadPos(size_t pos) noexcept
        {
            m_readPos = pos > Size() ? Size() : pos;
        }

        /**
         * @brief Set the position of the write head.
         * @param pos A byte offset.
         * @note The new write position will be clamped to >= Capacity().
         */
        void SetWritePos(size_t pos) noexcept
        {
            m_writePos = pos > Capacity() ? Capacity() : pos;
        }

        /**
         * @brief Write the contents of the BinaryBuffer to a file.
         * @param filePath The file path to write to.
        */
        void WriteToFile(const std::filesystem::path& filePath)
        {
            WriteBinaryFile(filePath, {m_data.data() + m_readPos, m_data.data() + m_writePos});
            m_readPos = m_writePos;
        }

        /**
         * @brief Release ownership of the underlying binary data.
         * @return A vector of bytes.
        */
        auto ReleaseBuffer() -> std::vector<uint8_t>
        {
            m_readPos = 0;
            m_writePos = 0;
            return std::move(m_data);
        }

    private:
        std::vector<uint8_t> m_data;
        size_t m_readPos = 0;
        size_t m_writePos = 0;

        void AssertRead(size_t bytes)
        {
            if (m_readPos + bytes > m_writePos )
            {
                throw NcError{"BinaryBuffer: attempt to read out of bounds."};
            }
        }

        void EnsureCapacity(size_t nBytes)
        {
            if (m_writePos + nBytes >= m_data.size())
            {
                Resize(m_data.size() + nBytes);
            }
        }
};
} // namespace nc::serialize
