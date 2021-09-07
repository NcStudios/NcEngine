#include "component/AudioSource.h"
#include "Ecs.h"
#include "assets/AssetManager.h"

namespace
{
    using namespace nc;

    float CalculateAttenuation(AttenuationFunction curveType, float innerRadius, float outerRadius, float squareDistance)
    {
        if(innerRadius * innerRadius > squareDistance)
            return 1.0f;

        switch(curveType)
        {
            case AttenuationFunction::Linear:
            {
                float distance = std::sqrt(squareDistance);
                float distanceRatio = (distance - innerRadius) / (outerRadius - innerRadius);
                return math::Clamp(1.0f - distanceRatio, 0.0f, 1.0f);
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
        m_rightChannel = std::span<const double>(soundClip.rightChannel);
        m_samplesPerChannel = soundClip.samplesPerChannel;

        m_properties.gain = math::Clamp(properties.gain, 0.0f, 1.0f);
    }

    void AudioSource::WriteSamples(double* buffer, size_t frames, const Vector3& listenerPosition, const Vector3& rightEar, const Vector3& leftEar)
    {
        if(!m_properties.spatialize)
        {
            WriteNonSpatialSamples(buffer, frames);
            return;
        }

        auto* registry = ActiveRegistry();
        auto* transform = registry->Get<Transform>(GetParentEntity());
        auto position = transform->GetPosition();

        auto squareDistance = SquareDistance(position, listenerPosition);
        auto squareOuterRadius = m_properties.outerRadius * m_properties.outerRadius;

        /** @todo may want to 'advance' sound even if it can't be heard */
        if(squareDistance > squareOuterRadius)
            return;

        auto attenuation = CalculateAttenuation(m_properties.attenuation, m_properties.innerRadius, m_properties.outerRadius, squareDistance);
        auto gain = attenuation * m_properties.gain;


        auto soundToLeftEar = Normalize(listenerPosition - position);
        auto normLeftEar = Normalize(leftEar);
        auto dotLeft = Dot(soundToLeftEar, normLeftEar);
        float leftPanFactor = 1.0f;
        if(dotLeft > 0.0f)
            leftPanFactor = math::Clamp(1.0f - dotLeft, 0.1f, 1.0f);

        auto soundToRightEar = Normalize(listenerPosition - position);
        auto normRightEar = Normalize(rightEar);
        auto dotRight = Dot(soundToRightEar, normRightEar);
        float rightPanFactor = 1.0f;
        if(dotRight > 0.0f)
            rightPanFactor = math::Clamp(1.0f - dotRight, 0.1f, 1.0f);

        for(size_t i = 0u; i < frames; ++i)
        {
            float sample = gain * 0.5f * (m_leftChannel[m_currentSampleIndex] + m_rightChannel[m_currentSampleIndex]);

            *buffer++ += sample * leftPanFactor;
            *buffer++ += sample * rightPanFactor;

            //*buffer++ += gain * leftPanFactor * m_leftChannel[m_currentSampleIndex];
            //*buffer++ += gain * rightPanFactor * m_rightChannel[m_currentSampleIndex];

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
        auto gain = m_properties.gain;

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