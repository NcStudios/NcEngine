#pragma once

#include "Graphics\GraphicsEngine\interface\GraphicsTypes.h"

#include <string>

namespace nc::graphics
{
struct StructuredBufferResourceDesc
{
    std::string resourceKey;
    Diligent::SHADER_TYPE shaderType;
    uint32_t maxElementCount;
    uint32_t initialElementCount;
};

struct TextureBufferResourceDesc
{
    std::string resourceKey;
    Diligent::SHADER_TYPE shaderType;
    uint32_t maxElementCount;
};

struct UniformBufferResourceDesc
{
    std::string resourceKey;
    Diligent::SHADER_TYPE shaderType;
};
} // namespace nc::graphics