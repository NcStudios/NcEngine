#include "ncengine/audio/AudioSource.h"
#include "ncengine/ecs/Registry.h"

namespace
{
auto GetClips(const std::vector<std::string>& paths) -> std::vector<nc::AudioClipView>
{
    auto out = std::vector<nc::AudioClipView>{};
    out.reserve(paths.size());
    std::ranges::transform(paths, std::back_inserter(out), [](const auto& path)
    {
        return nc::AcquireAudioClipAsset(path);
    });

    return out;
}

auto CalculateAttenuation(double innerRadius, double outerRadius, double squareDistance) -> double
{
    if(innerRadius * innerRadius > squareDistance)
        return 1.0;

    double distance = std::sqrt(squareDistance);
    double distanceRatio = (distance - innerRadius) / (outerRadius - innerRadius);
    return nc::Clamp(1.0 - distanceRatio, 0.0, 1.0);
}
} // anonymous namespace

namespace nc::audio
{
AudioSource::AudioSource(Entity entity,
                         std::vector<std::string> clips,
                         AudioSourceProperties properties)
    : ComponentBase{entity},
      m_clips{::GetClips(clips)},
      m_properties{properties},
      m_coldData{std::make_unique<AudioSourceColdData>(std::move(clips))}
{
    if (IsPlaying())
    {
        NC_ASSERT(!m_clips.empty(), "AudioSource must have at least one clip when initialized with 'Play' flag.");
        m_currentClipIndex = 0u;
    }
}

void AudioSource::Play(uint32_t clipIndex)
{
    NC_ASSERT(clipIndex < m_clips.size(), "Audio clip index out of bounds");
    m_currentClipIndex = clipIndex;
    m_currentSampleIndex = 0u;
    SetPlaying();
}

void AudioSource::PlayNext()
{
    NC_ASSERT(!m_clips.empty(), "AudioSource has no audio clips");
    if (m_currentClipIndex == NullClipIndex || ++m_currentClipIndex >= m_clips.size())
    {
        m_currentClipIndex = 0u;
    }

    m_currentSampleIndex = 0u;
    SetPlaying();
}

auto AudioSource::AddClip(std::string clip) -> uint32_t
{
    m_clips.push_back(AcquireAudioClipAsset(clip));
    m_coldData->assetPaths.push_back(std::move(clip));
    return static_cast<uint32_t>(m_clips.size() - 1);
}

void AudioSource::SetClip(uint32_t clipIndex, std::string path)
{
    NC_ASSERT(clipIndex < m_clips.size(), "Audio clip index out of bounds");
    m_clips[clipIndex] = AcquireAudioClipAsset(path);
    m_coldData->assetPaths.at(clipIndex) = std::move(path);
    if (m_currentClipIndex == clipIndex && IsPlaying())
    {
        m_currentClipIndex = NullClipIndex;
        SetStopped();
    }
}

void AudioSource::RemoveClip(uint32_t clipIndex)
{
    NC_ASSERT(clipIndex < m_clips.size(), "Audio clip index out of bounds");
    m_clips.erase(m_clips.begin() + clipIndex);
    m_coldData->assetPaths.erase(m_coldData->assetPaths.begin() + clipIndex);
    if (m_currentClipIndex == clipIndex)
    {
        m_currentClipIndex = NullClipIndex;
        SetStopped();
    }
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
    const double rightPresence = dotRight <= 0.0 ? 1.0 : Clamp(1.0 - dotRight, 0.2, 1.0);
    const double dotLeft = Dot(sourceToListener, -rightEar);
    const double leftPresence = dotLeft <= 0.0 ? 1.0 : Clamp(1.0 - dotLeft, 0.2, 1.0);
    const double attenuation = CalculateAttenuation(m_properties.innerRadius, m_properties.outerRadius, squareDistance);
    const double gain = 0.5 * attenuation * m_properties.gain;
    const auto& clip = m_clips.at(m_currentClipIndex);

    for(size_t i = 0u; i < frames; ++i)
    {
        if(++m_currentSampleIndex >= clip.samplesPerChannel)
        {
            m_currentSampleIndex = 0;
            if (!IsLooping())
            {
                SetStopped();
                return;
            }
        }

        const double sample = gain * (clip.leftChannel[m_currentSampleIndex] + clip.rightChannel[m_currentSampleIndex]);
        *buffer++ += sample * leftPresence;
        *buffer++ += sample * rightPresence;
    }
}

void AudioSource::WriteNonSpatialSamples(double* buffer, size_t frames)
{
    const double gain = 0.5 * m_properties.gain;
    const auto& clip = m_clips.at(m_currentClipIndex);

    for(size_t i = 0u; i < frames; ++i)
    {
        *buffer++ += gain * clip.leftChannel[m_currentSampleIndex];
        *buffer++ += gain * clip.rightChannel[m_currentSampleIndex];

        if(++m_currentSampleIndex >= clip.samplesPerChannel)
        {
            m_currentSampleIndex = 0;
            if (!IsLooping())
            {
                SetStopped();
                return;
            }
        }
    }
}
} // namespace nc::audio
