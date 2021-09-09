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

    /** @todo InverseSquareLaw, Log? */
    enum class AttenuationFunction
    {
        Linear
    };

    struct AudioSourceProperties
    {
        float gain = 1.0f;
        float innerRadius = 1.0f;
        float outerRadius = 15.0f;
        AttenuationFunction attenuation = AttenuationFunction::Linear;
        bool spatialize = false;
        bool loop = false;
    };

    class AudioSource : public ComponentBase
    {
        public:
            AudioSource(Entity entity, const std::string& path, AudioSourceProperties properties = AudioSourceProperties{});

            void WriteSamples(double* buffer, size_t frames, const Vector3& sourcePosition, const Vector3& listenerPosition, const Vector3& rightEar);
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

            void WriteNonSpatialSamples(double* buffer, size_t frames);
    };
}