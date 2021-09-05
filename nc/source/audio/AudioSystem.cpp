#include "AudioSystem.h"

namespace
{
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
    AudioSystem::AudioSystem(registry_type* registry)
        : m_registry{registry},
          m_rtAudio{}
    {
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

    int AudioSystem::WriteCallback(double* output, unsigned bufferFrames, RtAudioStreamStatus status)
    {
        /** @todo We should log underflows and/or display them in the editor. For now, I just want to
         *  quickly see when they happen. */
        if(status)
            std::cerr << "Audio stream underflow\n";
        
        auto sources = m_registry->ViewAll<AudioSource>();

        for(unsigned i = 0u; i < bufferFrames; ++i)
        {
            AudioSample sample;
            size_t sourceCount = 0u;

            for(auto& source : sources)
            {
                if(!source.IsPlaying())
                    continue;
                
                auto [left, right] = source.GetNextSample();
                sample.left += left;
                sample.right += right;
                ++sourceCount;
            }

            /** @todo There are more appropriate ways to do mixing. Not yet
             *  sure how we want to handle clipping and so on. */
            sample.left /= static_cast<double>(sourceCount);
            sample.right /= static_cast<double>(sourceCount);
            *output++ = sample.left;
            *output++ = sample.right;
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