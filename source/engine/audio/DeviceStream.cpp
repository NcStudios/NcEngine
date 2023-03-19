#include "DeviceStream.h"
#include "utility/Log.h"

#include "rtaudio/RtAudio.h"

namespace
{
constexpr auto g_outputChannelCount = 2u;
constexpr auto g_bufferFrames = 256u;
constexpr auto g_sampleRate = 44100u;
constexpr auto g_format = RTAUDIO_FLOAT64;
const auto g_nullDevice = nc::audio::AudioDevice{"NoDevice", nc::audio::InvalidDeviceId};

auto IsSuitableDevice(const RtAudio::DeviceInfo& deviceInfo) noexcept -> bool
{
    // TODO: #374 - Need to pin down exact outputChannel requirements
    return deviceInfo.probed && deviceInfo.outputChannels >= 2;
}
} // anonymous namespace

namespace nc::audio
{
DeviceStream::DeviceStream(uint32_t preferredDeviceId, StreamCallback_t callback, void* userData)
    : m_rtAudio{std::make_unique<RtAudio>()},
      m_activeDevice{g_nullDevice}
{
    m_rtAudio->showWarnings(true);
    OpenStream(preferredDeviceId, callback, userData);
}

DeviceStream::~DeviceStream() noexcept
{
    CloseStream();
}

auto DeviceStream::OpenStream(uint32_t preferredDeviceId, StreamCallback_t callback, void* userData) noexcept -> bool
{
    CloseStream();
    m_activeDevice = FindSuitableDevice(preferredDeviceId);
    if (m_activeDevice.id == InvalidDeviceId)
    {
        NC_LOG_ERROR("Failed to find a suitable audio output device");
        return false;
    }

    NC_LOG_INFO_FMT("Selected audio output device: {}", m_activeDevice.name);

    auto params = RtAudio::StreamParameters{m_activeDevice.id, g_outputChannelCount, 0};
    auto bufferFrames = g_bufferFrames;

    try
    {
        // We can always force float64 format, even if not available
        m_rtAudio->openStream(&params, nullptr, g_format, g_sampleRate, &bufferFrames, callback, userData);
        m_rtAudio->startStream();
    }
    catch(const RtAudioError& e)
    {
        NC_LOG_ERROR_FMT("Failed to open audio stream: {}", e.what());
        m_activeDevice = g_nullDevice;
        return false;
    }

    // TODO: handle this
    if (bufferFrames != g_bufferFrames)
    {
        return false;
    }

    NC_LOG_INFO("Succefully opened audio stream");
    return true;
}

void DeviceStream::CloseStream() noexcept
{
    m_activeDevice = g_nullDevice;
    if (!m_rtAudio->isStreamOpen())
    {
        return;
    }

    try
    {
        m_rtAudio->stopStream();
    }
    catch(const RtAudioError& e)
    {
        e.printMessage();
    }

    if (m_rtAudio->isStreamOpen())
    {
        m_rtAudio->closeStream();
    }
}

auto DeviceStream::FindSuitableDevice(uint32_t preferredDeviceId) noexcept -> AudioDevice
{
    auto requestedDefault = false;
    if (preferredDeviceId == DefaultDeviceId)
    {
        // Our default id is static, while its actual id may change at any point
        preferredDeviceId = m_rtAudio->getDefaultOutputDevice();
        requestedDefault = true;
    }

    auto rtDevice = m_rtAudio->getDeviceInfo(preferredDeviceId);
    if (::IsSuitableDevice(rtDevice))
    {
        return AudioDevice{std::move(rtDevice.name), preferredDeviceId};
    }

    // If preferred isn't available try the default, assuming we didn't already
    if (!requestedDefault)
    {
        preferredDeviceId = m_rtAudio->getDefaultOutputDevice();
        auto rtDevice = m_rtAudio->getDeviceInfo(preferredDeviceId);
        if (::IsSuitableDevice(rtDevice))
        {
            return AudioDevice{std::move(rtDevice.name), preferredDeviceId};
        }
    }

    // Probably don't have a valid device, but probe all just in case
    const auto devices = EnumerateDevices();
    return devices.empty() ? g_nullDevice : devices.front();
}

auto DeviceStream::EnumerateDevices() noexcept -> std::vector<AudioDevice>
{
    auto deviceCount = m_rtAudio->getDeviceCount();
    auto devices = std::vector<AudioDevice>{};
    devices.reserve(deviceCount);
    for (auto i = 0u; i < deviceCount; ++i)
    {
        auto rtDevice = m_rtAudio->getDeviceInfo(i);
        if (::IsSuitableDevice(rtDevice))
        {
            devices.emplace_back(std::move(rtDevice.name), i);
        }
    }

    return devices;
}
} // namespace nc::audio
