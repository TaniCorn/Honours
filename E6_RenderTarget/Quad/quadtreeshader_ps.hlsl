// Light pixel shader
// Calculate diffuse lighting for a single directional light (also texturing)

SamplerState sampler0 : register(s0);

struct InputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
};
cbuffer QuadBuffer : register(b0)
{
    float depth;
    float maxDepth;
    float2 padding;
};

float4 main(InputType input) : SV_TARGET
{
    float col =  depth / maxDepth;
    col *= 2;
    return float4(col,col, col, 1);

}



