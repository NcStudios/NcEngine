#pragma once

#include "CollateralCommon.h"

#include "audio_file/AudioFile.h"

namespace collateral
{
namespace sine
{
const auto filePath = collateralDirectory / "sine_c_e.wav";
const auto rawAsset = AudioFile<double>{filePath.string()};
const auto samplesPerChannel = rawAsset.samples.at(0).size();
const auto& leftChannel = rawAsset.samples.at(0);
const auto& rightChannel = rawAsset.samples.at(1);
} // namespace sine
} // namespace collateral