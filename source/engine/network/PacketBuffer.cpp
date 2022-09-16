#include "network/PacketBuffer.h"
#include "utility/NcError.h"

#include <cstring>

namespace nc::net
{
    template<> size_t WriteBuffer(uint8_t* out, size_t offset, bool in)
    {
        out[offset] = static_cast<uint8_t>(in);
        return offset + sizeof(in);
    }

    template<> size_t WriteBuffer(uint8_t* out, size_t offset, uint8_t in)
    {
        out[offset] = in;
        return offset + sizeof(in);
    }

    template<> size_t WriteBuffer(uint8_t* out, size_t offset, uint32_t in)
    {
        std::memcpy(out + offset, &in, sizeof(in));
        return offset + sizeof(in);
    }

    template<> size_t WriteBuffer(uint8_t* out, size_t offset, float in)
    {
        std::memcpy(out + offset, &in, sizeof(in));
        return offset + sizeof(in);
    }

    template<> size_t WriteBuffer(uint8_t* out, size_t offset, const char* in)
    {
        auto length = std::strlen(in) + 1u;
        std::memcpy(out + offset, in, length);
        return offset + length;
    }

    template<> size_t ReadBuffer(const uint8_t* const in, size_t offset, bool* out)
    {
        *out = static_cast<bool>(in[offset]);
        return offset + sizeof(bool);
    }

    template<> size_t ReadBuffer(const uint8_t* const in, size_t offset, uint8_t* out)
    {
        *out = in[offset];
        return offset + sizeof(uint8_t);
    }

    template<> size_t ReadBuffer(const uint8_t* const in, size_t offset, uint32_t* out)
    {
        std::memcpy(out, in + offset, sizeof(uint32_t));
        return offset + sizeof(uint32_t);
    }

    template<> size_t ReadBuffer(const uint8_t* const in, size_t offset, float* out)
    {
        std::memcpy(out, in + offset, sizeof(float));
        return offset + sizeof(float);
    }

    template<> size_t ReadBuffer(const uint8_t* const in, size_t offset, std::string* out)
    {
        *out = std::string{reinterpret_cast<const char*>(in + offset)};
        return offset + out->size() + 1u;
    }
}