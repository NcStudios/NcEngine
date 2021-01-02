// From Vertex Shader
struct VSOut
{
    float4 position : SV_POSITION;
};

float4 main(VSOut vertexOutput) : SV_Target
{
    return float4(1.0f, 0.0f, 0.0f, 1.0f);
}
