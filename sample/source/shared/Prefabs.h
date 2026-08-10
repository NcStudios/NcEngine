#pragma once

/**
 * @note To generate an updated assets file, run:
 * 
 * nc-convert -g sample/assets/manifest.json -o sample/source/shared -r nc::sample
 */
#include "GeneratedAssets.h"

#include "ncasset/DefaultAssets.h"
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

namespace font
{
extern asset::FontInfo ui;
} // namespace font

namespace material
{
extern MaterialDesc blue;
extern MaterialDesc green;
extern MaterialDesc guy2;
extern MaterialDesc ogre;
extern MaterialDesc tree;
extern MaterialDesc tree_leaves;
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
