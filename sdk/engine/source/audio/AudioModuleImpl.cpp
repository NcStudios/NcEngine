#include "AudioModuleImpl.h"
#include "audio/AudioSource.h"
#include "ecs/View.h"

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

int AudioSystemCallback(void* outputBuffer, void*, [[maybe_unused]] unsigned nBufferFrames, double, RtAudioStreamStatus status, void* userData)
{
    assert(nBufferFrames == BufferFrames);

    /** @todo We should log underflows and/or display them in the editor. For now, I just want to
     *  quickly see when they happen. */
    if(status) std::cerr << "Audio stream underflow\n";

    auto* system = static_cast<nc::audio::AudioModuleImpl*>(userData);
    return system->WriteToDeviceBuffer(static_cast<double*>(outputBuffer));
}

struct AudioModuleStub : public nc::AudioModule
{
    void RegisterListener(nc::Entity) noexcept override {}
    auto BuildWorkload() -> std::vector<nc::task::Job> override { return {}; }
    void Clear() noexcept override {}
};
} // namespace nc::audio

namespace nc::audio
{
auto BuildAudioModule(bool enableModule, Registry* reg) -> std::unique_ptr<AudioModule>
{
    if(enableModule)
    {
        return std::make_unique<AudioModuleImpl>(reg);
    }
    else
    {
        return std::make_unique<AudioModuleStub>();
    }
}

AudioModuleImpl::AudioModuleImpl(Registry* registry)
    : m_registry{registry},
        m_rtAudio{},
        m_readyBuffers{},
        m_staleBuffers{},
        m_bufferMemory(BufferLength * BufferCount, 0.0),
        m_readyMutex{},
        m_staleMutex{},
        m_listener{Entity::Null()}
{
    for(size_t i = 0u; i < BufferCount; ++i)
    {
        auto begin = m_bufferMemory.begin() + i * BufferLength;
        m_staleBuffers.emplace(begin, BufferLength);
    }

    /** @todo initializing RtAudio needs to be more robust
     *  -should have a way to select different devices
     *  -buffer frames should be in config
     *  -sample rate/channels/etc should be verified for the selected device */

    m_rtAudio.showWarnings(true);
    unsigned devices = m_rtAudio.getDeviceCount();
    if(devices < 1)
    {
        throw NcError("No audio devices found");
    }

    RtAudio::StreamParameters parameters
    {
        .deviceId = m_rtAudio.getDefaultOutputDevice(),
        .nChannels = OutputChannelCount,
        .firstChannel = 0
    };

    unsigned bufferFrames = BufferFrames;

    try
    {
        m_rtAudio.openStream(&parameters,
                                nullptr,
                                RTAUDIO_FLOAT64,
                                SampleRate,
                                &bufferFrames,
                                AudioSystemCallback,
                                static_cast<void*>(this));
        m_rtAudio.startStream();
    }
    catch(const RtAudioError& e)
    {
        e.printMessage();
        throw NcError("Failure starting audio stream");
    }

    if(bufferFrames != BufferFrames)
        throw NcError("Invalid number of buffer frames specified");
}

AudioModuleImpl::~AudioModuleImpl() noexcept
{
    Clear();

    try
    {
        m_rtAudio.stopStream();
    }
    catch(const RtAudioError& e)
    {
        e.printMessage();
    }

    if(m_rtAudio.isStreamOpen())
        m_rtAudio.closeStream();
}

void AudioModuleImpl::Clear() noexcept
{
    m_listener = Entity::Null();

    std::lock_guard lock{m_readyMutex};
    while(!m_readyBuffers.empty())
    {
        m_staleBuffers.push(m_readyBuffers.front());
        m_readyBuffers.pop();
    }
}

auto AudioModuleImpl::BuildWorkload() -> std::vector<task::Job>
{
    return std::vector<task::Job>
    {
        task::Job{ [this]{Run();}, "AudioModule", task::ExecutionPhase::Free }
    };
}

void AudioModuleImpl::RegisterListener(Entity listener) noexcept
{
    m_listener = listener;
}

int AudioModuleImpl::WriteToDeviceBuffer(double* output)
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

void AudioModuleImpl::Run()
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

void AudioModuleImpl::MixToBuffer(double* buffer)
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

auto AudioModuleImpl::ProbeDevices() -> std::vector<RtAudio::DeviceInfo>
{
    unsigned deviceCount = m_rtAudio.getDeviceCount();
    std::vector<RtAudio::DeviceInfo> out;
    for(unsigned i = 0u; i < deviceCount; ++i)
    {
        try
        {
            out.push_back(m_rtAudio.getDeviceInfo(i));
        }
        catch(const RtAudioError& e)
        {
            e.printMessage();
            break;
        }
    }

    return out;
}
} // namespace nc::audio
