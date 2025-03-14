#pragma once

/**
 * @note To generate an updated assets file, run:
 * 
 * nc-convert -g sample/assets/manifest.json -o sample/source/shared -r nc::sample
 */
#include "GeneratedAssets.h"

#include "ncengine/asset/AssetViews.h"
#include "ncengine/asset/NcAsset.h"
#include "ncengine/graphics/Material.h"
#include "ncengine/graphics/PostProcess.h"

namespace nc::sample
{
// Load Asset Files
void InitializeResources();

// Reload cached global asset views
void ReloadPrefabs();

// Audio Clips
namespace audio_clip
{
extern asset::AudioClipView Default;
} // namespace audio_clip

namespace font
{
extern asset::FontInfo ui;
} // namespace font

namespace mesh
{
extern asset::MeshView cube;
extern asset::MeshView sphere;
extern asset::MeshView capsule;
extern asset::MeshView plane;
extern asset::MeshView wheel;
} // namespace mesh

namespace material
{
extern MaterialDesc blue;
extern MaterialDesc cave;
extern MaterialDesc green;
extern MaterialDesc guy2;
extern MaterialDesc ogre;
extern MaterialDesc orange;
extern MaterialDesc purple;
extern MaterialDesc red;
extern MaterialDesc skeleton;
extern MaterialDesc teal;
extern MaterialDesc white;
extern MaterialDesc yellow;
} // namespace material

namespace post_process
{
extern OutlinePassProperties outline;
extern NoisePassProperties noise;
} // namespace post_process
} // namespace nc::sample
