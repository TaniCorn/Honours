// Texture pixel/fragment shader
// Basic fragment shader for rendering textured geometry
// Texture and sampler registers
Texture2D texture0 : register(t0);
SamplerState Sampler0 : register(s0);

struct InputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
};

float4 main(InputType input) : SV_TARGET
{
    float4 col = texture0.Sample(Sampler0, input.tex);
    return col;

}
