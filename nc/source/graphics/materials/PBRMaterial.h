#pragma once

#include "directx/math/DirectXMath.h"
#include "graphics/materials/Material.h"

#include <string>

namespace nc::graphics
{
    class PBRMaterial : public Material
    {
        public: 
            PBRMaterial() = default;
            PBRMaterial(const std::vector<std::string>& texturePaths);
    };
}