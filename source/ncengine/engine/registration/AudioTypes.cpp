#include "AudioTypes.h"
#include "ncengine/audio/AudioSource.h"

namespace nc
{
void RegisterAudioTypes(ecs::ComponentRegistry& registry, size_t maxEntities)
{
    Register<AudioSource>(
        registry,
        maxEntities,
        AudioSourceId,
        "AudioSource",
        ui::editor::AudioSourceUIWidget,
        CreateAudioSource,
        SerializeAudioSource,
        DeserializeAudioSource
    );
}
} // namespace nc
