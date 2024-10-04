#include "AudioConverter.h"
#include "utility/Path.h"

#include "audio_file/AudioFile.h"
#include "ncasset/Assets.h"
#include "ncutility/NcError.h"

#include <array>

namespace
{
const auto supportedFileExtensions = std::array<std::string, 1> { ".wav" };
}

namespace nc::convert
{
auto AudioConverter::ImportAudioClip(const std::filesystem::path& path) -> asset::AudioClip
{
    if (!ValidateInputFileExtension(path, supportedFileExtensions))
    {
        throw NcError("Invalid input file: ", path.string());
    }

    auto rawAsset = AudioFile<double>{};
    if(!rawAsset.load(path.string()))
    {
        throw NcError("Failure opening audio file: ", path.string());
    }

    if (rawAsset.samples.size() == 1)
    {
        return asset::AudioClip{
            rawAsset.samples.at(0).size(),
            rawAsset.samples.at(0),
            std::move(rawAsset.samples.at(0))
        };
    }

    return asset::AudioClip{
        rawAsset.samples.at(0).size(),
        std::move(rawAsset.samples.at(0)),
        std::move(rawAsset.samples.at(1))
    };
}
} // namespace nc::convert
