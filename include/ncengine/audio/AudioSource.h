/**
 * @file AudioSource.h
 * @copyright Jaremie Romer and McCallister Romer 2024
 */
#pragma once

#include "ncengine/asset/Assets.h"
#include "ncengine/ecs/Component.h"

#include <span>

namespace nc::audio
{
/** @brief Flags applying to all clips in an AudioSource. */
struct AudioSourceFlags
{
    static constexpr uint8_t None    = 0b00000000;
    static constexpr uint8_t Play    = 0b00000001;
    static constexpr uint8_t Loop    = 0b00000010;
    static constexpr uint8_t Spatial = 0b00000100;
};

/** @brief Properties applying to all clips in an AudioSource. */
struct AudioSourceProperties
{
    float gain = 1.0f;
    float innerRadius = 1.0f;
    float outerRadius = 15.0f;
    uint8_t flags = AudioSourceFlags::None;
};

/** @brief Indicates an invalid audio clip index. */
constexpr auto NullClipIndex = UINT32_MAX;

/** @brief Component managing audio clips.
 * 
 * An AudioSource may hold any number of audio clips, but only one may be playing at a time. When the Spatial flag
 * is set, the position of the owning Entity is used as the location of the AudioSource. For audio to be processed,
 * a listener Entity must be registered with the NcAudio module.
*/
class AudioSource : public ComponentBase
{
    public:
        AudioSource(Entity entity,
                    std::vector<std::string> clips,
                    AudioSourceProperties properties = AudioSourceProperties{});

        /** @brief Play the audio clip at a given index. */
        void Play(uint32_t clipIndex = 0ull);

        /** @brief Play the audio clip following the one most recently played (round-robin). */
        void PlayNext();

        /** @brief Check if a clip is currently playing. */
        auto IsPlaying() const noexcept -> bool { return m_properties.flags & AudioSourceFlags::Play; }

        /**
         * @brief Get the index of the most recently played clip.
         * @note Returns NullClipIndex if no clip has been played or the most recent clip was removed.
         */
        auto GetRecentClipIndex() const noexcept -> uint32_t { return m_currentClipIndex; }

        /** @brief Stop the currently playing audio clip. */
        void Stop() noexcept { m_properties.flags &= ~AudioSourceFlags::Play; }

        /**
         * @brief Add an audio clip.
         * @return The index where the clip was added.
         */
        auto AddClip(std::string clip) -> uint32_t;

        /** @brief Replace the audio clip at a given index. */
        void SetClip(uint32_t clipIndex, std::string clip);

        /**
         * @brief Remove the audio clip at a given index.
         * @note This will change the indices of any clips past the removed element.
         */
        void RemoveClip(uint32_t clipIndex);

        auto GetClips() const noexcept -> const std::vector<AudioClipView>& { return m_clips; }
        auto GetAssetPaths() const noexcept -> const std::vector<std::string>& { return m_coldData->assetPaths; }
        auto GetProperties() const noexcept -> const AudioSourceProperties& { return m_properties; }
        auto GetGain() const noexcept -> float { return m_properties.gain; }
        auto GetInnerRadius() const noexcept -> float { return m_properties.innerRadius; }
        auto GetOuterRadius() const noexcept -> float { return m_properties.outerRadius; }
        auto IsSpatial() const noexcept -> bool { return m_properties.flags & AudioSourceFlags::Spatial; }
        auto IsLooping() const noexcept -> bool { return m_properties.flags & AudioSourceFlags::Loop; }

        void SetProperties(const AudioSourceProperties& properties) noexcept { m_properties = properties; }
        void SetGain(float gain) noexcept { m_properties.gain = gain; }
        void SetInnerRadius(float radius) noexcept { m_properties.innerRadius = radius; }
        void SetOuterRadius(float radius) noexcept { m_properties.outerRadius = radius; }

        void SetSpatial(bool spatialize) noexcept
        {
            m_properties.flags = spatialize
                ? m_properties.flags | AudioSourceFlags::Spatial
                : m_properties.flags & ~AudioSourceFlags::Spatial;
        }

        void SetLooping(bool loop) noexcept
        {
            m_properties.flags = loop
                ? m_properties.flags | AudioSourceFlags::Loop
                : m_properties.flags & ~AudioSourceFlags::Loop;
        }

    private:
        struct AudioSourceColdData
        {
            std::vector<std::string> assetPaths;
        };

        std::vector<AudioClipView> m_clips;
        uint32_t m_currentClipIndex = NullClipIndex;
        uint32_t m_currentSampleIndex = 0u;
        AudioSourceProperties m_properties;
        std::unique_ptr<AudioSourceColdData> m_coldData;

        void SetPlaying() noexcept { m_properties.flags |= AudioSourceFlags::Play; }
        void SetStopped() noexcept { m_properties.flags &= ~AudioSourceFlags::Play; }
        void WriteSpatialSamples(double* buffer, size_t frames, const Vector3& sourcePosition, const Vector3& listenerPosition, const Vector3& rightEar);
        void WriteNonSpatialSamples(double* buffer, size_t frames);

        friend class NcAudioImpl;
};
} // namespace nc::audio
