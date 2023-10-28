#include "DeviceStream.h"
#include "utility/Log.h"

#include "rtaudio/RtAudio.h"

#include <mutex>

namespace
{
constexpr auto g_format = RTAUDIO_FLOAT64;
const auto g_nullDevice = nc::audio::AudioDevice{"NoDevice", nc::audio::InvalidDeviceId};

auto ToString(RtAudioErrorType type) noexcept -> std::string_view
{
    switch(type)
    {
        case RTAUDIO_NO_ERROR:          return "NO_ERROR";
        case RTAUDIO_WARNING:           return "WARNING";
        case RTAUDIO_UNKNOWN_ERROR:     return "UNKNOWN_ERROR";
        case RTAUDIO_NO_DEVICES_FOUND:  return "NO_DEVICES_FOUND";
        case RTAUDIO_INVALID_DEVICE:    return "INVALID_DEVICE";
        case RTAUDIO_DEVICE_DISCONNECT: return "DEVICE_DISCONNECT";
        case RTAUDIO_MEMORY_ERROR:      return "MEMORY_ERROR";
        case RTAUDIO_INVALID_PARAMETER: return "INVALID_PARAMETER";
        case RTAUDIO_INVALID_USE:       return "INVALID_USE";
        case RTAUDIO_DRIVER_ERROR:      return "DRIVER_ERROR";
        case RTAUDIO_SYSTEM_ERROR:      return "SYSTEM_ERROR";
        case RTAUDIO_THREAD_ERROR:      return "THREAD_ERROR";
        default:                        return "UNKNOWN";
    }
}

auto IsSuitableDevice(const RtAudio::DeviceInfo& deviceInfo) noexcept -> bool
{
    // TODO: #374 - Need to pin down exact outputChannel requirements
    return deviceInfo.outputChannels >= 2;
}
} // anonymous namespace

namespace nc::audio
{
class DeviceStream::ErrorContext
{
    public:
        auto GetStatus() const noexcept -> nc::audio::StreamStatus
        {
            auto lock = std::lock_guard{m_mutex};
            return m_status;
        }

        void SetStatus(nc::audio::StreamStatus status) noexcept
        {
            auto lock = std::lock_guard{m_mutex};
            m_status = status;
        }

        auto MakeCallback() noexcept -> RtAudioErrorCallback
        {
            return [this](RtAudioErrorType type, const std::string& errorText)
            {
                NC_LOG_ERROR("Audio device error: '{}'\n\tError Text: '{}'", ToString(type), errorText);
                SetStatus(nc::audio::StreamStatus::Failed);
            };
        }

    private:
        mutable std::mutex m_mutex;
        nc::audio::StreamStatus m_status = nc::audio::StreamStatus::Open;
};

DeviceStream::DeviceStream(const StreamParameters& params)
    : m_errorContext{std::make_unique<ErrorContext>()},
      m_rtAudio{std::make_unique<RtAudio>(RtAudio::Api::UNSPECIFIED, m_errorContext->MakeCallback())},
      m_activeDevice{g_nullDevice}
{
    NC_LOG_INFO("Audio API: {}", m_rtAudio->getApiName(m_rtAudio->getCurrentApi()));
    OpenStream(params);
}

DeviceStream::~DeviceStream() noexcept
{
    CloseStream();
}

auto DeviceStream::OpenStream(const StreamParameters& params) -> bool
{
    CloseStream();
    m_activeDevice = FindSuitableDevice(params.deviceId);
    if (m_activeDevice.id == InvalidDeviceId)
    {
        NC_LOG_ERROR("Failed to find a suitable audio output device");
        return false;
    }

    NC_LOG_INFO("Selected audio output device: '{}'", m_activeDevice.name);
    auto rtParams = RtAudio::StreamParameters{m_activeDevice.id, params.channelCount, 0};
    m_bufferFrames = params.bufferFrames;
    const auto result = m_rtAudio->openStream(&rtParams,
                                              nullptr,
                                              g_format,
                                              params.sampleRate,
                                              &m_bufferFrames,
                                              params.callback,
                                              params.userData,
                                              nullptr);
    if (result)
    {
        NC_LOG_ERROR("Failed to open audio stream: '{}'", ::ToString(result));
        m_activeDevice = g_nullDevice;
        return false;
    }

    if (auto startResult = m_rtAudio->startStream())
    {
        NC_LOG_ERROR("Failed to start audio stream: '{}'", ::ToString(startResult));
        m_activeDevice = g_nullDevice;
        return false;
    }

    NC_LOG_INFO("Succefully opened audio stream");
    m_errorContext->SetStatus(StreamStatus::Open);
    return true;
}

void DeviceStream::CloseStream() noexcept
{
    m_activeDevice = g_nullDevice;
    m_errorContext->SetStatus(StreamStatus::Closed);
    if (!m_rtAudio->isStreamOpen())
    {
        return;
    }

    if (auto result = m_rtAudio->isStreamRunning() ? m_rtAudio->stopStream() : RTAUDIO_NO_ERROR)
    {
        NC_LOG_ERROR("Failed to stop audio stream: '{}'", ::ToString(result));
        return;
    }

    if (m_rtAudio->isStreamOpen())
    {
        m_rtAudio->closeStream();
    }
}

auto DeviceStream::GetStreamStatus() const noexcept -> StreamStatus
{
    return m_errorContext->GetStatus();
}

auto DeviceStream::EnumerateDevices() noexcept -> std::vector<AudioDevice>
{
    auto devices = std::vector<AudioDevice>{};
    for (auto id : m_rtAudio->getDeviceIds())
    {
        auto rtDevice = m_rtAudio->getDeviceInfo(id);
        if (::IsSuitableDevice(rtDevice))
        {
            devices.emplace_back(std::move(rtDevice.name), rtDevice.ID);
        }
    }

    return devices;
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
        rtDevice = m_rtAudio->getDeviceInfo(preferredDeviceId);
        if (::IsSuitableDevice(rtDevice))
        {
            return AudioDevice{std::move(rtDevice.name), preferredDeviceId};
        }
    }

    // Probably don't have a valid device, but probe all just in case
    const auto devices = EnumerateDevices();
    return devices.empty() ? g_nullDevice : devices.front();
}

auto DeviceStream::GetStreamTime() const noexcept -> double
{
    return m_rtAudio->isStreamOpen() ? m_rtAudio->getStreamTime() : 0.0;
}

void DeviceStream::SetStreamTime(double time) noexcept
{
    if (m_rtAudio->isStreamOpen())
    {
        m_rtAudio->setStreamTime(time);
    }
}
} // namespace nc::audio
