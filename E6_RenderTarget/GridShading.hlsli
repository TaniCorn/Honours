static const int OneDimensionGrid[8] = { 1, 1, 1, 1, 1, 1, 1, 1 };

static const int TwoDimensionGrid[3 * 3] =
{
    1, 1, 1,
     1, 0, 1,
     1, 1, 1
};

static const int ThreeDimensionGrid[3 * 3 * 3] =
{
    1, 1, 1,
    1, 0, 1,
    1, 1, 1,
    1, 1, 1,
    1, 0, 1,
    1, 1, 1,
     1, 1, 1,
     1, 0, 1,
     1, 1, 1
};

#include "Tracer.hlsli"
float4 Render3DGrid(float3 camPos, float3 rayVector, bool raymarch)
{
    for (int x = 0; x < 3; x++)
    {
        for (int y = 0; y < 3; y++)
        {
            for (int z = 0; z < 3; z++)
            {
                if (ThreeDimensionGrid[x * 9 + y * 3 + z] == 0)
                {
                    continue;
                }
        
        
                int hit = 0;
                if (raymarch)
                {
                    hit = raymarchHit(camPos.xyz, rayVector, float3(x, y, z), 500, 0.1f, false, 0.1f);
                }
                else
                {
                    hit = raySphere(camPos.xyz, rayVector, float3(x, y, z), 0.1f);
                }

                if (hit >= 0)
                {
                    return float4(0, 1.0f, 1.0f, 1.0f);
                }
                else
                {
                    continue;
                }
            }
        }

    }
    return float4(rayVector.x, rayVector.y, rayVector.z, 1);
}
float4 Render2DGrid(float3 camPos, float3 rayVector, bool raymarch)
{
    for (int x = 0; x < 3; x++)
    {
        for (int y = 0; y < 3; y++)
        {
            if (TwoDimensionGrid[x * 3 + y] == 0)
            {
                continue;
            }
        
        
            int hit = 0;
            if (raymarch)
            {
                hit = raymarchHit(camPos.xyz, rayVector, float3(x, y, 0), 500, 0.1f, false, 0.1f);
            }
            else
            {
                hit = raySphere(camPos.xyz, rayVector, float3(x, y, 0), 0.1f);
            }

            if (hit >= 0)
            {
                return float4(0, 1.0f, 1.0f, 1.0f);
            }
            else
            {
                continue;
            }
        }

    }
    return float4(rayVector.x, rayVector.y, rayVector.z, 1);
    
}
float4 Render1DGrid(float3 camPos, float3 rayVector, bool raymarch)
{
    for (int i = 0; i < 8; i++)
    {
        if (OneDimensionGrid[i] == 0)
        {
            continue;
            
        }
        
        int hit = 0;
        if(raymarch)
        {
            hit = raymarchHit(camPos.xyz, rayVector, float3(0, 0, i), 500, 0.1f, false, 0.1f);
        }
        else
        {
            hit = raySphere(camPos.xyz, rayVector, float3(0, 0, i), 0.1f);
        }

        if (hit >= 0)
        {
            return float4(0, 1.0f, 1.0f, 1.0f);
        }
        else
        {
            continue;
        }
    }
    return float4(rayVector.x, rayVector.y, rayVector.z, 1);
}
