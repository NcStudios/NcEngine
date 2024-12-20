#pragma once

#include "graphics2/ShaderTypes.h"

namespace nc::graphics
{
struct ParticleRenderState
{
    BufferUpdateInfo<ParticleData> particleData;
    asset::MeshView mesh;
};
} // namespace nc::graphics
