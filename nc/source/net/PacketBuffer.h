#pragma once

#include <stdint.h>
#include <string>

namespace nc::net
{
    struct PacketBuffer
    {
        const static uint32_t bufferSize = 64u;
        uint32_t size = 0u;
        uint8_t data[bufferSize];
    };

    template<class T, class ... Args>
    size_t WriteBuffer(uint8_t* out, size_t offset, T first, Args ... args)
    {
        auto newOffset = WriteBuffer(out, offset, first);
        return WriteBuffer(out, newOffset, args...);
    }

    template<class T, class ... Args>
    size_t ReadBuffer(const uint8_t* const in, size_t offset, T first, Args ... args)
    {
        auto newOffset = ReadBuffer(in, offset, first);
        return ReadBuffer(in, newOffset, args...);
    }

    template<> size_t WriteBuffer(uint8_t* out, size_t offset, bool in);
    template<> size_t WriteBuffer(uint8_t* out, size_t offset, uint8_t in);
    template<> size_t WriteBuffer(uint8_t* out, size_t offset, uint32_t in);
    template<> size_t WriteBuffer(uint8_t* out, size_t offset, float in);
    template<> size_t WriteBuffer(uint8_t* out, size_t offset, const char* in);

    template<> size_t ReadBuffer(const uint8_t* const in, size_t offset, bool* out);
    template<> size_t ReadBuffer(const uint8_t* const in, size_t offset, uint8_t* out);
    template<> size_t ReadBuffer(const uint8_t* const in, size_t offset, uint32_t* out);
    template<> size_t ReadBuffer(const uint8_t* const in, size_t offset, float* out);
    template<> size_t ReadBuffer(const uint8_t* const in, size_t offset, std::string* out);
} // end namespace nc::net