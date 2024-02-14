// Texture pixel/fragment shader
// Basic fragment shader for rendering textured geometry
// Texture and sampler registers
Texture2D texture0 : register(t0);
SamplerState Sampler0 : register(s0);
cbuffer MatrixBuffer : register(b0)
{
    matrix viewMatrix;
    matrix projectionMatrix;
    matrix invViewMatrix;
    matrix invProjMatrix;
    matrix worldMatrix;
    matrix invWorldMatrix;
};


struct InputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    float4 cameraPosition : TEXCOORD1;
};

#include "GridShading.hlsli"

float4 main(InputType input) : SV_TARGET
{
    float3 camPos = input.cameraPosition.xyz;
    float2 res = float2(1184, 636);
    float fov = 1.0;
    
    float4 v = UVPositionCalculation(res, input.tex, projectionMatrix, fov);
    float3 rayVector = CalculateViewVector(v, viewMatrix, worldMatrix);
    //return Render3DGrid(camPos, rayVector, true, float4(1, 1, 1, 1));
    return Render2DGrid(camPos, rayVector, false);
    return Render1DGrid(camPos, rayVector, false);

}
