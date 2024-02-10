#pragma once

#include <cstdint>

namespace nc::graphics
{
using shader_stage = uint8_t;

struct ShaderStage
{
    static constexpr shader_stage Vertex   = 0b00000000; // Resources using this stage will bind to the vertex shader.
    static constexpr shader_stage Fragment = 0b00000001; // Resources using this stage will bind to the fragment shader.
};
}