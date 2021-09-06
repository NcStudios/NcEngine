#include "AudioSystem.h"
#include "Audio.h"

namespace
{
    nc::audio::AudioSystem* g_audioSystem = nullptr;
    constexpr unsigned OutputChannelCount = 2u;
    constexpr unsigned SampleRate = 44100u;
    constexpr unsigned BufferFrames = 256u;

    int AudioSystemCallback(void* outputBuffer, void*, unsigned nBufferFrames, double, RtAudioStreamStatus status, void* userData)
    {
        auto* system = static_cast<nc::audio::AudioSystem*>(userData);
        return system->WriteCallback(static_cast<double*>(outputBuffer), nBufferFrames, status);
    }
}

namespace nc::audio
{
    void RegisterListener(Entity entity)
    {
        IF_THROW(!g_audioSystem, "RegisterAudioListener - AudioSystem is not set");
        g_audioSystem->RegisterListener(entity);
    }

    AudioSystem::AudioSystem(registry_type* registry)
        : m_registry{registry},
          m_rtAudio{},
          m_listener{Entity::Null()}
    {
        g_audioSystem = this;

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
            throw std::runtime_error("AudioSystem - Failure starting audio stream");
        }
    }

    AudioSystem::~AudioSystem()
    {
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

    void AudioSystem::Clear()
    {
        m_listener = Entity::Null();
    }

    void AudioSystem::RegisterListener(Entity listener)
    {
        m_listener = listener;
    }

    int AudioSystem::WriteCallback(double* output, unsigned bufferFrames, RtAudioStreamStatus status)
    {
        if(!m_listener.Valid())
            return 0;

        auto* listenerTransform = ActiveRegistry()->Get<Transform>(m_listener);
        if(!listenerTransform)
            throw std::runtime_error("AudioSystem::WriteCallback - Invalid listener registered");
        
        auto listenerPosition = listenerTransform->GetPosition();
        auto rightEar = listenerTransform->Right();
        auto leftEar = -rightEar;

        /** @todo We should log underflows and/or display them in the editor. For now, I just want to
         *  quickly see when they happen. */
        if(status)
            std::cerr << "Audio stream underflow\n";

        for(unsigned i = 0; i < bufferFrames * 2u; ++i)
        {
            output[i] = 0.0f;
        }

        auto sources = m_registry->ViewAll<AudioSource>();

        for(auto& source : sources)
        {
            if(!source.IsPlaying())
                continue;
            
            source.WriteSamples(output, bufferFrames, listenerPosition, rightEar, leftEar);
        }

        return 0;
    }

    auto AudioSystem::ProbeDevices() -> std::vector<RtAudio::DeviceInfo>
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