#include "ecs/component/AudioSource.h"
#include "ecs/Registry.h"
#include "assets/AssetService.h"

namespace
{
    using namespace nc;

    double CalculateAttenuation(AttenuationFunction curveType, double innerRadius, double outerRadius, double squareDistance)
    {
        if(innerRadius * innerRadius > squareDistance)
            return 1.0;

        switch(curveType)
        {
            case AttenuationFunction::Linear:
            {
                double distance = std::sqrt(squareDistance);
                double distanceRatio = (distance - innerRadius) / (outerRadius - innerRadius);
                return math::Clamp(1.0 - distanceRatio, 0.0, 1.0);
            }
        }

        throw NcError("Unknown AttenuationFunction");
    }
}

namespace nc
{
    AudioSource::AudioSource(Entity entity, const std::string& path, AudioSourceProperties properties)
        : ComponentBase{entity},
          m_audioClip{AssetService<AudioClipView>::Get()->Acquire(path)},
          m_currentSampleIndex{0u},
          m_properties{properties},
          m_playing{false}
          #ifdef NC_EDITOR_ENABLED
          , m_audioClipPath{path}
          #endif
    {
        m_properties.gain = math::Clamp(properties.gain, 0.0f, 1.0f);
    }

    void AudioSource::SetClip(const std::string& path)
    {
        m_audioClip = AssetService<AudioClipView>::Get()->Acquire(path);
        m_currentSampleIndex = 0u;
    }

    void AudioSource::SetProperties(const AudioSourceProperties& properties)
    {
        m_properties = properties;
    }

    void AudioSource::WriteSpatialSamples(double* buffer, size_t frames, const Vector3& sourcePosition, const Vector3& listenerPosition, const Vector3& rightEar)
    {
        const auto squareDistance = SquareDistance(sourcePosition, listenerPosition);
        const auto squareOuterRadius = m_properties.outerRadius * m_properties.outerRadius;

        /** @todo may want to 'advance' sound even if it can't be heard */
        if(squareDistance > squareOuterRadius)
            return;

        const auto sourceToListener = Normalize(listenerPosition - sourcePosition);
        const double dotRight = Dot(sourceToListener, rightEar);
        const double rightPresence = dotRight <= 0.0 ? 1.0 : math::Clamp(1.0 - dotRight, 0.2, 1.0);
        const double dotLeft = Dot(sourceToListener, -rightEar);
        const double leftPresence = dotLeft <= 0.0 ? 1.0 : math::Clamp(1.0 - dotLeft, 0.2, 1.0);
        const double attenuation = CalculateAttenuation(m_properties.attenuation, m_properties.innerRadius, m_properties.outerRadius, squareDistance);
        const double gain = 0.5 * attenuation * m_properties.gain;

        for(size_t i = 0u; i < frames; ++i)
        {
            if(++m_currentSampleIndex >= m_audioClip.samplesPerChannel)
            {
                m_currentSampleIndex = 0;
                if(!m_properties.loop)
                {
                    m_playing = false;
                    return;
                }
            }

            const double sample = gain * (m_audioClip.leftChannel[m_currentSampleIndex] + m_audioClip.rightChannel[m_currentSampleIndex]);
            *buffer++ += sample * leftPresence;
            *buffer++ += sample * rightPresence;
        }
    }

    void AudioSource::WriteNonSpatialSamples(double* buffer, size_t frames)
    {
        const double gain = 0.5 * m_properties.gain;

        for(size_t i = 0u; i < frames; ++i)
        {
            *buffer++ += gain * m_audioClip.leftChannel[m_currentSampleIndex];
            *buffer++ += gain * m_audioClip.rightChannel[m_currentSampleIndex];

            if(++m_currentSampleIndex >= m_audioClip.samplesPerChannel)
            {
                m_currentSampleIndex = 0;
                if(!m_properties.loop)
                {
                    m_playing = false;
                    return;
                }
            }
        }
    }
}