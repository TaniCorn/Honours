Texture2D texture0 : register(t0);
SamplerState sampler0 : register(s0);
float4 toGreyScale(float4 color)
{
    float4 grey;
    float average = color.x + color.y + color.z;
    average = average / 3;
    return float4(average, average, average, 1);

}
struct InputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
};
float4 main(InputType input) : SV_TARGET
{
    float4 textureColour;
    float4 lightColour;

    //return float4(1, 1, 1, 1);
	// Sample the texture. Calculate light intensity and colour, return light*texture for final pixel colour.
    textureColour = texture0.Sample(sampler0, input.tex);
    textureColour = toGreyScale(textureColour);
    return textureColour;;
}