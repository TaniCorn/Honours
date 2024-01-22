Texture2D texture0 : register(t0);
SamplerState sampler0 : register(s0);
struct InputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
    float4 camera : TEXCOORD1;
    float2 screen : TEXCOORD2;
};
float4 main(InputType input) : SV_TARGET
{
    float4 textureColour;
    float4 lightColour;
    float4 cameraPos = float4(input.camera.xyzw);
    //cameraPos.xyz /= cameraPos.w;
    //cameraPos.xz *= float2(0.5, -0.5);
    //cameraPos.xz += 0.5f;
    //cameraPos.xz *= float2(input.screen.x, input.screen.y);
    float buf = 5.5f;
   // cameraPos = normalize(cameraPosition);
    //return float4(1, 1, 1, 1);
	// Sample the texture. Calculate light intensity and colour, return light*texture for final pixel colour.
    textureColour = texture0.Sample(sampler0, input.tex);
   // textureColour = toGreyScale(textureColour);
   // if (input.tex.x <= (0.51 - cameraPos.x) && input.tex.x >= (0.49 - cameraPos.x) && input.tex.y <= (0.51 + cameraPos.z) && input.tex.y >= (0.49 + cameraPos.z))
    //   return float4(1, 1, 1, 1);
    
    

    if ((input.position.x <= (cameraPos.x + buf)) && (input.position.x >= (cameraPos.x - buf)) 
        && (input.position.y <= (cameraPos.z + buf)) && (input.position.y >= (cameraPos.z - buf)))
    {
        return float4(1, 1, 1, 1);

    }
    return textureColour;;
}