#include "NcAudioImpl.h"
#include "audio/AudioSource.h"
#include "config/Config.h"
#include "ecs/View.h"
#include "utility/Log.h"

#include "optick/optick.h"
#include <cstring>
#include <iostream>

namespace
{
constexpr unsigned OutputChannelCount = 2u;
constexpr unsigned SampleRate = 44100u;
constexpr unsigned BufferCount = 4u;
constexpr unsigned BufferFrames = 256u;
constexpr unsigned BufferLength = BufferFrames * OutputChannelCount;
constexpr unsigned BufferSizeInBytes = BufferLength * sizeof(double);

auto BuildBufferQueue(std::span<double> bufferPool) -> std::queue<std::span<double>>
{
    auto queue = std::queue<std::span<double>>{};
    for(auto i = 0ull; i < BufferCount; ++i)
    {
        auto begin = bufferPool.begin() + i * BufferLength;
        queue.emplace(begin, BufferLength);
    }

    return queue;
}

int AudioSystemCallback(void* outputBuffer, void*, [[maybe_unused]] unsigned nBufferFrames, double, unsigned status, void* userData)
{
    assert(nBufferFrames == BufferFrames);

    /** @todo We should log underflows and/or display them in the editor. For now, I just want to
     *  quickly see when they happen. */
    if(status) std::cerr << "Audio stream underflow\n";

    auto* system = static_cast<nc::audio::NcAudioImpl*>(userData);
    return system->WriteToDeviceBuffer(static_cast<double*>(outputBuffer));
}

struct NcAudioStub : public nc::audio::NcAudio
{
    nc::audio::AudioDevice nullDevice{"NoDevice", nc::audio::InvalidDeviceId};

    void RegisterListener(nc::Entity) noexcept override{}
    auto EnumerateOutputDevices() noexcept -> std::vector<nc::audio::AudioDevice> override { return {}; }
    auto GetOutputDevice() const noexcept -> const nc::audio::AudioDevice& override { return nullDevice; }
    auto SetOutputDevice(uint32_t) noexcept -> bool override { return false; }
    auto BuildWorkload() -> std::vector<nc::task::Job> override { return {}; }
    void Clear() noexcept override {}
};
} // anonymous namespace

namespace nc::audio
{
auto BuildAudioModule(const config::AudioSettings& settings, Registry* reg) -> std::unique_ptr<NcAudio>
{
    if(settings.enabled)
    {
        NC_LOG_TRACE("Building NcAudio module");
        return std::make_unique<NcAudioImpl>(reg);
    }

    NC_LOG_TRACE("Audio disabled - building NcAudio stub");
    return std::make_unique<NcAudioStub>();
}

NcAudioImpl::NcAudioImpl(Registry* registry)
    : m_registry{registry},
      m_bufferMemory(BufferLength * BufferCount, 0.0),
      m_readyBuffers{},
      m_staleBuffers{::BuildBufferQueue(m_bufferMemory)},
      m_readyMutex{},
      m_staleMutex{},
      m_deviceStream{0u, ::AudioSystemCallback, static_cast<void*>(this)},
      m_listener{Entity::Null()}
{
}

NcAudioImpl::~NcAudioImpl() noexcept
{
    Clear();
}

void NcAudioImpl::Clear() noexcept
{
    m_listener = Entity::Null();

    std::lock_guard lock{m_readyMutex};
    while(!m_readyBuffers.empty())
    {
        m_staleBuffers.push(m_readyBuffers.front());
        m_readyBuffers.pop();
    }
}

auto NcAudioImpl::BuildWorkload() -> std::vector<task::Job>
{
    NC_LOG_TRACE("Building NcAudio workload");
    return std::vector<task::Job>
    {
        task::Job{ [this]{Run();}, "AudioModule", task::ExecutionPhase::Free }
    };
}

void NcAudioImpl::RegisterListener(Entity listener) noexcept
{
    NC_LOG_TRACE_FMT("Registering audio listener: {}", listener.Index());
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

auto NcAudioImpl::SetOutputDevice(uint32_t deviceId) noexcept -> bool
{
    return m_deviceStream.OpenStream(deviceId, ::AudioSystemCallback, static_cast<void*>(this));
}

int NcAudioImpl::WriteToDeviceBuffer(double* output)
{
    // empty check before lock is only safe with 1 consumer
    if(m_readyBuffers.empty())
    {
        std::memset(output, 0, BufferSizeInBytes);
        return 0;
    }

    std::span<double> buffer;

    {
        std::lock_guard lock{m_readyMutex};
        buffer = m_readyBuffers.front();
        m_readyBuffers.pop();
    }

    std::memcpy(output, buffer.data(), BufferSizeInBytes);

    {
        std::lock_guard lock{m_staleMutex};
        m_staleBuffers.push(buffer);
    }

    return 0;
}

void NcAudioImpl::Run()
{
    OPTICK_CATEGORY("AudioModule", Optick::Category::Audio);
    if(!m_listener.Valid())
        return;

    std::span<double> buffer;

    for(size_t i = 0u; i < BufferCount; ++i)
    {
        if(m_staleBuffers.empty())
            return;

        {
            std::lock_guard lock{m_staleMutex};
            buffer = m_staleBuffers.front();
            m_staleBuffers.pop();
        }

        MixToBuffer(buffer.data());

        {
            std::lock_guard lock{m_readyMutex};
            m_readyBuffers.push(buffer);
        }
    }
}

void NcAudioImpl::MixToBuffer(double* buffer)
{
    std::memset(buffer, 0, BufferSizeInBytes);

    const auto* listenerTransform = m_registry->Get<Transform>(m_listener);
    if(!listenerTransform)
        throw NcError("Invalid listener registered");

    const auto listenerPosition = listenerTransform->Position();
    const auto rightEar = listenerTransform->Right();

    for(auto& source : View<AudioSource>{m_registry})
    {
        if(!source.IsPlaying())
            continue;

        if(source.IsSpatial())
        {
            auto* transform = m_registry->Get<Transform>(source.ParentEntity());
            source.WriteSpatialSamples(buffer, BufferFrames, transform->Position(), listenerPosition, rightEar);
        }
        else
        {
            source.WriteNonSpatialSamples(buffer, BufferFrames);
        }
    }
}
} // namespace nc::audio
