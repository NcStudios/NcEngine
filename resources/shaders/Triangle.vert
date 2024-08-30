#version 140

layout (location = 0) out vec4 OutPosition;
layout (location = 0) out vec3 OutColor;

void main()
{
    vec4 position[3];
    position[0] = vec4(-0.5, -0.5, 0.0, 1.0);
    position[1] = vec4( 0.0, +0.5, 0.0, 1.0);
    position[2] = vec4(+0.5, -0.5, 0.0, 1.0);

    vec3 color[3];
    color[0] = vec3(1.0f, 0.0f, 0.0f);
    color[1] = vec3(0.0f, 1.0f, 0.0f);
    color[2] = vec3(0.0f, 0.0f, 1.0f);

    OutPosition = position[gl_VertexID];
    OutColor = color[gl_VertexID];
}