#pragma once

#include "Component.h"

#include <span>

namespace nc
{
    struct AudioSample
    {
        double left = 0.0f;
        double right = 0.0f;
    };

    struct AudioSourceProperties
    {
        double gain = 1.0f;
        bool loop = false;
    };

    class AudioSource : public ComponentBase
    {
        public:
            AudioSource(Entity entity, const std::string& path, AudioSourceProperties properties = AudioSourceProperties{});

            auto GetNextSample() -> AudioSample;
            void Play() { m_playing = true; m_currentSampleIndex = 0u; }
            void Stop() { m_playing = false; m_currentSampleIndex = 0u; }
            bool IsPlaying() const { return m_playing; }

        private:
            std::span<const double> m_leftChannel;
            std::span<const double> m_rightChannel;
            size_t m_samplesPerChannel;
            size_t m_currentSampleIndex;
            AudioSourceProperties m_properties;
            bool m_playing;
    };
}