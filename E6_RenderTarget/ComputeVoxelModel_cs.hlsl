struct VoxelOctree
{
    float3 TopLeftFrontPosition; // 4bytes*3 = 12bytes
    float3 BottomRightBackPosition; //12 bytes
    uint RGB; //12 bytes
    //uint TLF; //4bytes
    //uint TRF;
    //uint BLF;
    //uint BRF;
    
    //uint TLB;
    //uint TRB;
    //uint BLB;
    //uint BRB;
    uint Octants[8];
    //4bytes * 8 = 32bytes
    //32 + 12 + 12 + 12 = 68bytes per octree
};
struct Voxel
{
    float3 voxPosition;
    uint color;
};
#define TLF 0
#define TRF 1
#define BLF 2
#define BRF 3
#define TLB 4
#define TRB 5
#define BLB 6
#define BRB 7


cbuffer InvMatrixBuffer : register(b0)
{
    matrix viewMatrix;
    matrix projectionMatrix;
    matrix invViewMatrix;
    matrix invProjMatrix;
    matrix worldMatrix;
    matrix invWorldMatrix;
};

cbuffer CameraBuffer : register(b1)
{
    float3 cameraPosition;
    float padding;
};

#include "GridShading.hlsli"

// Input and output structures
Texture2D gInput : register(t0);
StructuredBuffer<VoxelOctree> voxelOctree : register(t1);
RWTexture2D<float4> gOutput : register(u0);

int raySphere(float3 rayPosition, float3 rayDirection, float3 spherePosition)
{
    //https://www.youtube.com/watch?v=4NshnkzOdI0
    //https://www.youtube.com/watch?v=Qz0KTGYJtUk
    //(bx^2 + by^2)t^2 + (2(axbx + ayby))t + (ax^2 +ay^2 - r^2) = 0;
    //a = ray origin
    //b = ray direction
    //r = radius
    //t = hit distance
    
    //-b +- sqrt(b^2 - 4ac) / 2a
    
    //The upcoming values are calculated with test scenario sphere(0,0,5), rayd(0,0,1), rayp(0,0,0)
    float sphereRadius = 2.0f;
    float3 offsetRayOrigin = rayPosition - spherePosition; //-5
    
    float a = dot(rayDirection, rayDirection); //Always 1?
    float b = 2 * dot(offsetRayOrigin, rayDirection); //-5*2= -10
    float c = dot(offsetRayOrigin, offsetRayOrigin) - sphereRadius * sphereRadius; //25 - 2^2 = 21
    
    float discriminant = b * b - 4 * a * c; //100 - 4 * 21 * 1 = 100 - 84 = 16
    
    if (discriminant >= 0)
    {
        float dist = (-b - sqrt(discriminant)) / (2 * a); // (-(-10) - sqrt(16))/2*1 = (10 - 4) / 2 = 6 / 2 = 3...??
        
        if (dist >= 0)
        {
            return 1;
        }
        else
        {
            return -1;
        }

    }
    return -1;

}

[numthreads(16, 16, 1)]
void main(int3 groupThreadID : SV_GroupThreadID,
	int3 dispatchThreadID : SV_DispatchThreadID)
{
    //float c = gInput[int2(0, 0)] + 0.5f;
    //float2 res = float2(1184, 636);
    //float4 col = float4(1, 1, 1, 1);
    
    //for (int x = 0; x < res.x; x++)
    //{
    //    for (int y = 0; y < res.y/2; y++)
    //    {
    //        gOutput[int2(x, y)] = col;
    //    }
    //}
    int x = dispatchThreadID.x;
    int y = dispatchThreadID.y;
    
    if (dispatchThreadID.y > 636)
    {
        return;
    }
    //float3 camPos = cameraPosition.xyz;
    //float2 res = float2(1184, 636);
    //float fov = 1.0;
    //        float texx = float(x) / float(res.x);
    //        float texy = float(y) / float(res.y);
    //        float4 v = UVPositionCalculation(res, float2(texx, texy), projectionMatrix, fov);
    //        float3 rayVector = CalculateViewVector(v, viewMatrix, worldMatrix);
    //        int hit = raySphere(camPos.xyz, rayVector, float3(0, 0, 5));
                
    //        if (hit >= 0)
    //        {
    //            gOutput[int2(x, y)] = float4(1, 1, 1, 1);
    //        }
    //        else
    //        {
    //            gOutput[int2(x, y)] = float4(rayVector.x, rayVector.y, rayVector.z, 1);
    //        }

    
    float3 camPos = cameraPosition.xyz;
    float2 res = float2(1184, 636);
    float fov = 1.0;
    float texx = float(x) / float(res.x);
    float texy = float(y) / float(res.y);
    float4 v = UVPositionCalculation(res, float2(texx, texy), projectionMatrix, fov);
    float3 rayVector = CalculateViewVector(v, viewMatrix, worldMatrix);
    
    for (int i = 0; )
        int hit = raySphere(camPos, rayVector, )
    gOutput[int2(x, y)] = Render3DGrid(camPos, rayVector, false, float4(1, 1, 1, 1));

}


//Indirection texture
//A(0,0) B(1,0)C(2,0)D(3,0)
//__________________________________
//| - - | - - | - - | - - |
//| - - | - - | - - | - - |
//__________________________________
