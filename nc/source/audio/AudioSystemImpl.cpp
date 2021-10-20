#include "AudioSystemImpl.h"

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

        auto* system = static_cast<nc::audio::AudioSystemImpl*>(userData);
        return system->WriteToDeviceBuffer(static_cast<double*>(outputBuffer));
    }
}

namespace nc::audio
{
    AudioSystemImpl::AudioSystemImpl(registry_type* registry)
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
            throw std::runtime_error("AudioSystem - No devices found");
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
            throw std::runtime_error("AudioSystemImpl - Failure starting audio stream");
        }

        if(bufferFrames != BufferFrames)
            throw std::runtime_error("AudioSystemImpl - Invalid number of buffer frames specified");
    }

    AudioSystemImpl::~AudioSystemImpl()
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

    void AudioSystemImpl::Clear() noexcept
    {
        m_listener = Entity::Null();

        std::lock_guard lock{m_readyMutex};
        while(!m_readyBuffers.empty())
        {
            m_staleBuffers.push(m_readyBuffers.front());
            m_readyBuffers.pop();
        }
    }

    void AudioSystemImpl::RegisterListener(Entity listener)
    {
        m_listener = listener;
    }

    int AudioSystemImpl::WriteToDeviceBuffer(double* output)
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

    void AudioSystemImpl::Update()
    {
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

    void AudioSystemImpl::MixToBuffer(double* buffer)
    {
        std::memset(buffer, 0, BufferSizeInBytes);

        const auto* listenerTransform = m_registry->Get<Transform>(m_listener);
        if(!listenerTransform)
            throw std::runtime_error("AudioSystemImpl::MixToBuffer - Invalid listener registered");
        
        const auto listenerPosition = listenerTransform->GetPosition();
        const auto rightEar = listenerTransform->Right();

        for(auto& source : m_registry->ViewAll<AudioSource>())
        {
            if(!source.IsPlaying())
                continue;
            
            if(source.IsSpatial())
            {
                auto* transform = m_registry->Get<Transform>(source.GetParentEntity());
                source.WriteSpatialSamples(buffer, BufferFrames, transform->GetPosition(), listenerPosition, rightEar);
            }
            else
            {
                source.WriteNonSpatialSamples(buffer, BufferFrames);
            }
        }
    }

    auto AudioSystemImpl::ProbeDevices() -> std::vector<RtAudio::DeviceInfo>
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
}