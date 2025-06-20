Texture2D texture0 : register(t0);
SamplerState Sampler0 : register(s0);

struct InputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
};

float4 main(InputType input) : SV_TARGET
{
    return float4(1, 1, 0, 1);
    // This will simply sample the texture, this should recieve the raytraced svo
    float4 col = texture0.Sample(Sampler0, input.tex);
    return col;
}
