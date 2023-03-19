#pragma once

#include "audio/NcAudio.h"

#include <memory>
#include <vector>

class RtAudio;

namespace nc::audio
{
using StreamCallback_t = int (*)(void* outputBuffer, void* inputBuffer, unsigned nBufferFrames, double streamTime, unsigned status, void* userData);

class DeviceStream
{
    public:
        DeviceStream(uint32_t preferredDeviceId, StreamCallback_t callback, void* userData);
        ~DeviceStream() noexcept;

        auto GetDevice() const noexcept -> const AudioDevice&
        {
            return m_activeDevice;
        }

        auto EnumerateDevices() noexcept -> std::vector<AudioDevice>;
        auto FindSuitableDevice(uint32_t preferredDeviceId) noexcept -> AudioDevice;
        auto OpenStream(uint32_t preferredDeviceId, StreamCallback_t callback, void* userData) noexcept -> bool;
        void CloseStream() noexcept;

    private:
        std::unique_ptr<RtAudio> m_rtAudio;
        AudioDevice m_activeDevice;
};
} // namespace nc::audio
