#include "component/AudioSource.h"
#include "assets/AssetManager.h"

namespace nc
{
    AudioSource::AudioSource(Entity entity, const std::string& path, AudioSourceProperties properties)
        : ComponentBase{entity},
          m_leftChannel{},
          m_rightChannel{},
          m_samplesPerChannel{},
          m_currentSampleIndex{0u},
          m_properties{properties},
          m_playing{false}
    {
        const auto& soundClip = AssetManager::AcquireSoundClip(path);
        m_leftChannel = std::span<const double>{soundClip.leftChannel};
        m_rightChannel = std::span<const double>(soundClip.rightChannel);
        m_samplesPerChannel = soundClip.samplesPerChannel;

        m_properties.gain = math::Clamp(properties.gain, 0.0, 1.0);
    }

    auto AudioSource::GetNextSample() -> AudioSample
    {
        auto gain = m_properties.gain;

        AudioSample out
        {
            .left = gain * m_leftChannel[m_currentSampleIndex],
            .right = gain * m_rightChannel[m_currentSampleIndex]
        };

        if(++m_currentSampleIndex >= m_samplesPerChannel)
        {
            m_currentSampleIndex = 0;
            if(!m_properties.loop)
            {
                m_playing = false;
            }
        }

        return out;
    }
}