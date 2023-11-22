// Texture pixel/fragment shader
// Basic fragment shader for rendering textured geometry
// Texture and sampler registers
Texture2D texture0 : register(t0);
SamplerState Sampler0 : register(s0);
#include "../Tracer.hlsli"
cbuffer MatrixBuffer : register(b0)
{
    matrix viewMatrix;
    matrix projectionMatrix;
    matrix invViewMatrix;
    matrix invProjMatrix;
    matrix worldMatrix;
    matrix invWorldMatrix;
};
//#define UVVERSION 0
//#define VIEWCOORDS 0//Visualise the viewcoords
//#define RAYMARCH 0//Use the raymarch algorithm or raytrace algorithm
//#if RAYMARCH
//#define RAYMARCHBOX 1//Raymarches box or sphere
//#endif


//#define ORTHO 0
//#define STEPS 5000
//#define STEP_SIZE 0.001
//#if ORTHO
//    #define RADIUS 50
//#else
//    #define RADIUS 1
//#endif





struct InputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    float4 cameraPosition : TEXCOORD1;
};


float4 main(InputType input) : SV_TARGET
{
    float3 camPos = input.cameraPosition.xyz;
    float2 res = float2(1184, 636);
    float fov = 1.0;
    
    float4 v = UVPositionCalculation(res, input.tex, projectionMatrix, fov);
    float3 rayVector = CalculateViewVector(v, viewMatrix, worldMatrix);

    int hit = raymarchHit(camPos.xyz, rayVector, float3(0, 0, 5), 5000, 0.01f, true, 1.0f);
    //int hit = SphereTracing(camPos.xyz, rayVector, float3(0, 0, 5), 5000, 1.0f);
    //int hit = raySphere(camPos.xyz, rayVector, float3(0, 0, 5), 1.0f);
    

    if (hit >= 0)
    {
        return float4(0, 1.0f, 1.0f, 1.0f);
    }
    else
    {
        return float4(rayVector.x, rayVector.y, rayVector.z, 1);
    }

}
    //float3 rayposition = { x - w/2,y - h/2, 0 };
    //float3 raydirection = { 0, 0, 1 };
    //float rays = raymarchHit(rayposition, float3(0, 0, 5), raydirection);
    //if (rays >= 0)
    //{
    //    return float4(0, 1, 0, 1);
    //}
    //else
    //{
    //    return float4(1, 0, 0, 1);
    //}