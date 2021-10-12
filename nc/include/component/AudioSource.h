#pragma once

#include "Component.h"
#include "Assets.h"

#include <span>

namespace nc
{
    namespace audio { class AudioSystem; }

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
        NC_ENABLE_IN_EDITOR(AudioSource)
        
        public:
            AudioSource(Entity entity, const std::string& path, AudioSourceProperties properties = AudioSourceProperties{});

            void SetClip(const std::string& path);
            void SetProperties(const AudioSourceProperties& properties);
            void Play() { m_playing = true; m_currentSampleIndex = 0u; }
            void Stop() { m_playing = false; m_currentSampleIndex = 0u; }
            bool IsPlaying() const { return m_playing; }
            bool IsSpatial() const { return m_properties.spatialize; }

        private:
            SoundClipView m_soundClip;
            size_t m_currentSampleIndex;
            AudioSourceProperties m_properties;
            bool m_playing;

            #ifdef NC_EDITOR_ENABLED
            std::string m_soundClipPath;
            #endif

            void WriteSpatialSamples(double* buffer, size_t frames, const Vector3& sourcePosition, const Vector3& listenerPosition, const Vector3& rightEar);
            void WriteNonSpatialSamples(double* buffer, size_t frames);

            friend audio::AudioSystem;
    };
}