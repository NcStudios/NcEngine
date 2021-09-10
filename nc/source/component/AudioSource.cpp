#include "component/AudioSource.h"
#include "Ecs.h"
#include "assets/AssetManager.h"

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

        throw std::runtime_error("CalculateAttenuation - Unknown AttenuationFunction");
    }
}

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
        m_rightChannel = std::span<const double>{soundClip.rightChannel};
        m_samplesPerChannel = soundClip.samplesPerChannel;

        m_properties.gain = math::Clamp(properties.gain, 0.0f, 1.0f);
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
            const double sample = gain * (m_leftChannel[m_currentSampleIndex] + m_rightChannel[m_currentSampleIndex]);

            *buffer++ += sample * leftPresence;
            *buffer++ += sample * rightPresence;

            if(++m_currentSampleIndex >= m_samplesPerChannel)
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

    void AudioSource::WriteNonSpatialSamples(double* buffer, size_t frames)
    {
        const double gain = 0.5 * m_properties.gain;

        for(size_t i = 0u; i < frames; ++i)
        {
            *buffer++ += gain * m_leftChannel[m_currentSampleIndex];
            *buffer++ += gain * m_rightChannel[m_currentSampleIndex];

            if(++m_currentSampleIndex >= m_samplesPerChannel)
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