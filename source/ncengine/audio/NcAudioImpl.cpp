#include "NcAudioImpl.h"
#include "ncengine/config/Config.h"
#include "ncengine/debug/Profile.h"
#include "ncengine/ecs/Ecs.h"
#include "ncengine/utility/Log.h"

#include <cstring>

namespace
{
int AudioSystemCallback(void* outputBuffer, void*, unsigned nBufferFrames, double, unsigned, void* userData)
{
    auto* system = static_cast<nc::audio::NcAudioImpl*>(userData);
    return system->WriteToDeviceBuffer(static_cast<double*>(outputBuffer), nBufferFrames);
}

auto CreateStreamParams(uint32_t deviceId, uint32_t bufferFrames, nc::audio::NcAudioImpl* impl) -> nc::audio::StreamParameters
{
    return nc::audio::StreamParameters
    {
        deviceId,
        bufferFrames,
        nc::audio::AudioBuffer::OutputChannelCount,
        nc::audio::AudioBuffer::SampleRate,
        ::AudioSystemCallback,
        static_cast<void*>(impl)
    };
}

struct NcAudioStub : public nc::NcAudio
{
    nc::AudioDevice nullDevice{"NoDevice", nc::InvalidAudioDeviceId};
    nc::Signal<const nc::AudioDevice&> nullSignal;

    void OnBuildTaskGraph(nc::task::UpdateTasks& update, nc::task::RenderTasks&)
    {
        update.Add(
            nc::update_task_id::AudioSourceUpdate,
            "AudioSourceUpdate(stub)",
            []{},
            {nc::update_task_id::CommitStagedChanges}
        );
    }

    void RegisterListener(nc::Entity) noexcept override{}
    auto GetStreamTime() const noexcept -> double override { return 0.0; }
    void SetStreamTime(double) noexcept override {}
    auto EnumerateOutputDevices() noexcept -> std::vector<nc::AudioDevice> override { return {}; }
    auto GetOutputDevice() const noexcept -> const nc::AudioDevice& override { return nullDevice; }
    auto SetOutputDevice(uint32_t) noexcept -> bool override { return false; }
    auto OnChangeOutputDevice() noexcept -> nc::Signal<const nc::AudioDevice&>& override { return nullSignal; }
};
} // anonymous namespace

namespace nc
{
auto BuildAudioModule(const config::AudioSettings& settings, ecs::ExplicitEcs<Entity, Transform, AudioSource> gameState) -> std::unique_ptr<NcAudio>
{
    if(settings.enabled)
    {
        NC_LOG_TRACE("Building NcAudio module");
        return std::make_unique<audio::NcAudioImpl>(settings, gameState);
    }

    NC_LOG_TRACE("Audio disabled - building NcAudio stub");
    return std::make_unique<NcAudioStub>();
}

namespace audio
{
NcAudioImpl::NcAudioImpl(const config::AudioSettings& settings, ecs::ExplicitEcs<Entity, Transform, AudioSource> gameState)
    : m_gameState{gameState},
      m_deviceStream{::CreateStreamParams(DefaultAudioDeviceId, settings.bufferFrames, this)},
      m_buffer{m_deviceStream.GetBufferFrames()},
      m_listener{Entity::Null()},
      m_configBufferFrames{settings.bufferFrames}
{
}

NcAudioImpl::~NcAudioImpl() noexcept
{
    Clear();
}

void NcAudioImpl::Clear() noexcept
{
    m_buffer.Clear();
    m_listener = Entity::Null();
}

void NcAudioImpl::OnBuildTaskGraph(task::UpdateTasks& update, task::RenderTasks&)
{
    NC_LOG_TRACE("Building NcAudio Tasks");
    update.Add(
        update_task_id::AudioSourceUpdate,
        "AudioSourceUpdate",
        [this]{ Run(); },
        {update_task_id::CommitStagedChanges}
    );
}

void NcAudioImpl::RegisterListener(Entity listener) noexcept
{
    NC_LOG_TRACE("Registering audio listener: {}", listener.Index());
    m_listener = listener;
}

auto NcAudioImpl::EnumerateOutputDevices() noexcept -> std::vector<AudioDevice>
{
    return m_deviceStream.EnumerateDevices();
}

auto NcAudioImpl::GetOutputDevice() const noexcept -> const AudioDevice&
{
    return m_deviceStream.GetDevice();
}

auto NcAudioImpl::SetOutputDevice(AudioDeviceId deviceId) noexcept -> bool
{
    const auto params = ::CreateStreamParams(deviceId, m_configBufferFrames, this);
    const auto result = m_deviceStream.OpenStream(params);
    ApplyDeviceChange();
    return result;
}

auto NcAudioImpl::OnChangeOutputDevice() noexcept -> Signal<const AudioDevice&>&
{
    return m_outputDeviceChanged;
}

auto NcAudioImpl::GetStreamTime() const noexcept -> double
{
    return m_deviceStream.GetStreamTime();
}

void NcAudioImpl::SetStreamTime(double time) noexcept
{
    m_deviceStream.SetStreamTime(time);
}

int NcAudioImpl::WriteToDeviceBuffer(double* output, [[maybe_unused]] uint32_t bufferFrames)
{
    assert(bufferFrames == m_buffer.FramesPerBuffer());
    const auto buffer = m_buffer.AcquireReadyBuffer();
    const auto bytes = m_buffer.BytesPerBuffer();
    if (buffer.data)
    {
        std::memcpy(output, buffer.data, bytes);
        m_buffer.MarkBufferStale(buffer);
    }
    else
    {
        std::memset(output, 0, bytes);
    }

    return 0;
}

void NcAudioImpl::Run()
{
    NC_PROFILE_TASK("AudioModule", ProfileCategory::Audio);
    if (!CheckStreamStatus() || !m_listener.Valid())
    {
        return;
    }

    const auto bufferFrames = m_buffer.FramesPerBuffer();
    for (auto bufferNumber = 0u; bufferNumber < AudioBuffer::BufferSlices; ++bufferNumber)
    {
        const auto buffer = m_buffer.AcquireStaleBuffer();
        if (buffer.data)
        {
            MixToBuffer(buffer.data, bufferFrames);
            m_buffer.MarkBufferReady(buffer);
            continue;
        }

        break;
    }
}

void NcAudioImpl::MixToBuffer(double* buffer, uint32_t bufferFrames)
{
    std::memset(buffer, 0, m_buffer.BytesPerBuffer());
    const auto& listenerTransform = m_gameState.Get<Transform>(m_listener);
    const auto listenerPosition = listenerTransform.Position();
    const auto rightEar = listenerTransform.Right();

    for (auto& source : m_gameState.GetAll<AudioSource>())
    {
        if (!source.IsPlaying())
        {
            continue;
        }

        if (source.IsSpatial())
        {
            auto& transform = m_gameState.Get<Transform>(source.ParentEntity());
            source.WriteSpatialSamples(buffer, bufferFrames, transform.Position(), listenerPosition, rightEar);
        }
        else
        {
            source.WriteNonSpatialSamples(buffer, bufferFrames);
        }
    }
}

auto NcAudioImpl::CheckStreamStatus() -> bool
{
    switch (m_deviceStream.GetStreamStatus())
    {
        case StreamStatus::Open:      return true;
        case StreamStatus::Failed:    return TryFailureRecovery();
        default:                      return false;
    }
}

void NcAudioImpl::ApplyDeviceChange()
{
    const auto bufferFrames = m_deviceStream.GetBufferFrames();
    if (bufferFrames != m_buffer.FramesPerBuffer())
    {
        m_buffer.Resize(bufferFrames);
    }

    m_outputDeviceChanged.Emit(m_deviceStream.GetDevice());
}

auto NcAudioImpl::TryFailureRecovery() -> bool
{
    const auto lastKnownDevice = m_deviceStream.GetDevice().id;
    const auto preferredDevice = lastKnownDevice == InvalidAudioDeviceId
        ? DefaultAudioDeviceId
        : lastKnownDevice;

    NC_LOG_TRACE("Attempting to to reopen an AudioDevice stream (device {}).", preferredDevice);
    const auto result = m_deviceStream.OpenStream(::CreateStreamParams(preferredDevice, m_configBufferFrames, this));
    if (!result)
    {
        NC_LOG_TRACE("Failed to reopen an AudioDevice stream. Abandoning AudioDevice.");
        m_deviceStream.AbandomStream();
        m_outputDeviceChanged.Emit(m_deviceStream.GetDevice());
        return false;
    }

    if (m_deviceStream.GetDevice().id != lastKnownDevice)
    {
        ApplyDeviceChange();
    }

    return true;
}
} // namespace audio
} // namespace nc
