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

namespace mesh
{
extern asset::MeshView Cube;
extern asset::MeshView Sphere;
extern asset::MeshView Capsule;
extern asset::MeshView Plane;
extern asset::MeshView Wheel;
} // namespace mesh

namespace material
{
extern MaterialDesc Blue;
extern MaterialDesc Cave;
extern MaterialDesc Default;
extern MaterialDesc Green;
extern MaterialDesc Guy2;
extern MaterialDesc Ogre;
extern MaterialDesc Orange;
extern MaterialDesc Purple;
extern MaterialDesc Red;
extern MaterialDesc Skeleton;
extern MaterialDesc Teal;
extern MaterialDesc Yellow;
} // namespace material

extern asset::FontInfo UIFont;

namespace post_process
{
extern OutlinePassProperties Outline;
extern NoisePassProperties Noise;
} // namespace post_process
} // namespace nc::sample
