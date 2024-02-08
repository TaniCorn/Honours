// Vertical compute blurr, based on Frank Luna's example.

// Input and output structures
Texture2D gInput : register(t0);
RWTexture2D<float4> gOutput : register(u0);

[numthreads(1, 1, 1)]
void main(int3 groupThreadID : SV_GroupThreadID,
	int3 dispatchThreadID : SV_DispatchThreadID)
{
    float c = gInput[int2(0, 0)] + 0.5f;
    float4 col = float4(c,c,c,c);
    gOutput[int2(0, 0)] = col;
}