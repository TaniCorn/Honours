cbuffer CB : register(b0)
{
    unsigned int g_iLevel;
    unsigned int g_iLevelMask;
    unsigned int g_iWidth;
    unsigned int g_iHeight;
};
cbuffer FL : register(b1)
{
    float3 faultLine;
    float pad;
    float3 p1;
    float pad2;
}

StructuredBuffer<float> Input : register(t0);
RWStructuredBuffer<float> Output : register(u0);

[numthreads(1, 1, 1)]
void main(uint3 Gid : SV_GroupID,
                      uint3 DTid : SV_DispatchThreadID,
                      uint3 GTid : SV_GroupThreadID,
                      uint GI : SV_GroupIndex)
{
    //Output[0] = Input[3];
    for (int i = 0; i < 10; i++)
    {
        Output[i] = 0.1 + Input[i];
    }

    //Data[DTid.x] = (Input[DTid.x]) + 1;
    //Input.Load(5);
}

float3 CrossProduct(const float3 v1, const float3 v2)
{
    float3 result;
    result.x = (v1.y * v2.z - v1.z * v2.y);
    result.y = (v1.z * v2.x - v1.x * v2.z);
    result.z = (v1.x * v2.y - v1.y * v2.x);
    return
result;
}
void ChooseEdgePoint(int resolution)
{
    int realRes = sqrt(resolution);
    for (int i = 0; i < realRes; i++)
    {
        for (int j = 0; j < realRes; j++)
        {
            float3 currentVector = float3(p1.x - i, 0, p1.z - j);
            float3 result = CrossProduct(faultLine, currentVector);

            if (result.y > 0)
                result[(j * realRes) + i] += 0.5;
            else
                result[(j * realRes) + i] -= 0.5;
        }
        
    }

}

