#pragma once

#include "audio/NcAudio.h"

#include <memory>
#include <vector>

class RtAudio;

namespace nc::audio
{
using StreamCallback = int (*)(void* outputBuffer, void* inputBuffer, unsigned nBufferFrames, double streamTime, unsigned status, void* userData);

struct StreamParameters
{
    uint32_t deviceId;
    uint32_t bufferFrames;
    uint32_t channelCount;
    uint32_t sampleRate;
    StreamCallback callback;
    void* userData;
};

enum class StreamStatus : uint8_t
{
    Open = 0,
    Closed = 1,
    Failed = 1
};

class DeviceStream
{
    public:
        DeviceStream(const StreamParameters& params);
        ~DeviceStream() noexcept;

        auto OpenStream(const StreamParameters& params) -> bool;
        void CloseStream() noexcept;
        auto GetStreamStatus() const noexcept -> StreamStatus;
        auto GetStreamTime() const noexcept -> double;
        void SetStreamTime(double time) noexcept;
        auto EnumerateDevices() noexcept -> std::vector<AudioDevice>;

        auto GetDevice() const noexcept -> const AudioDevice&
        {
            return m_activeDevice;
        }

        auto GetBufferFrames() const noexcept -> uint32_t
        {
            return m_bufferFrames;
        }

    private:
        class ErrorContext;
        std::unique_ptr<ErrorContext> m_errorContext;
        std::unique_ptr<RtAudio> m_rtAudio;
        AudioDevice m_activeDevice;
        uint32_t m_bufferFrames = 0u;

        auto FindSuitableDevice(uint32_t preferredDeviceId) noexcept -> AudioDevice;
};
} // namespace nc::audio
