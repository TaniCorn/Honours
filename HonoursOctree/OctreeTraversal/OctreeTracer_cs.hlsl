#include "../VoxelOctree.hlsli"
struct Ray
{
    float3 RayPos;
    float3 RayDirection;
};
#define MAX_STACK_SIZE 500
#define MAX_ITERATIONS 75
#define MAX_DEBUG_ITERATIONS 9999

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
cbuffer ViewModeBuffer : register(b2)
{
    int ViewMode;
    int ViewDepth;
    int heat;
    float pad;
};
#include "../GridShading.hlsli"

// Input and output structures
Texture2D gInput : register(t0);
StructuredBuffer<VoxelOctree> voxelOctree : register(t1);
RWTexture2D<float4> gOutput : register(u0);

int DoesRayIntersect(Ray r, StructuredBuffer<VoxelOctree> Octree)
{
    // Traverse the octree to find the leaf node
    VoxelOctree currentNode = Octree[0];
    if (!rayBox(r.RayPos, r.RayDirection, currentNode.TopLeftFrontPosition, currentNode.BottomRightBackPosition))
    {
        return 0;
    }
    int stackIndexes[MAX_STACK_SIZE];
    int stackTop = 0;
    stackIndexes[stackTop] = 0;
    stackTop++;
    for (int iterations = 0; iterations < MAX_ITERATIONS && stackTop > 0; iterations++)
    {
        int OctreeStride = stackIndexes[--stackTop];
        currentNode = Octree[OctreeStride];
    
            // If the node is not a leaf node, push its child nodes onto the stack
        if (currentNode.Depth == ViewDepth)
        {
            return iterations; // Intersection found
        }
        for (int i = 0; i < 8; i++)
        {
            int Stride = currentNode.Octants[i];
            if (Stride != 0)
            {
                VoxelOctree childNode = Octree[Stride];
                if (childNode.Depth > ViewDepth)
                {
                    break;
                }
                if (rayBox(r.RayPos, r.RayDirection, childNode.TopLeftFrontPosition, childNode.BottomRightBackPosition))
                {
                    stackIndexes[stackTop] = Stride;
                    stackTop++;
                }
    

            }
        }
    }
    if (heat)
    {
        if (iterations == MAX_ITERATIONS)
        {
            return -1;
        }
        else
        {
           return iterations; // No intersection found//Hit bounding octree
        }
    }
        return 0; // No intersection found//Hit bounding octree
}

bool RenderAtDepth(Ray r, StructuredBuffer<VoxelOctree> Octree)
{
    for (int iterations = 0; iterations < MAX_DEBUG_ITERATIONS; iterations++)
    {
        if (Octree[iterations].Depth == ViewDepth)
        {
            if (rayBox(r.RayPos, r.RayDirection, Octree[iterations].TopLeftFrontPosition, Octree[iterations].BottomRightBackPosition))
            {
                return true;
            }
        }
    }
    return false;
}
bool RenderBox(Ray r, StructuredBuffer<VoxelOctree> Octree)
{
    for (int iterations = 0; iterations < MAX_DEBUG_ITERATIONS; iterations++)
    {
        if (Octree[iterations].RGB == ViewDepth)
        {
            if (rayBox(r.RayPos, r.RayDirection, Octree[iterations].TopLeftFrontPosition, Octree[iterations].BottomRightBackPosition))
            {
                return true;
            }
        }
    }
    return false;
}
bool HitWireframe(Ray r, float3 tlf, float3 brb)
{
    bool hitflag = false;
    //Top
    float3 tlb = float3(tlf.x+0.1, tlf.y-0.1, brb.z+0.1);
    float3 blf = float3(tlf.x + 0.1, brb.y + 0.1, tlf.z + 0.1);
    float3 trf = float3(brb.x - 0.1, tlf.y - 0.1, tlf.z + 0.1);
    
    float3 blb = float3(tlf.x - 0.1, brb.y - 0.1, brb.z - 0.1);
    float3 brf = float3(brb.x - 0.1, brb.y + 0.1, tlf.z - 0.1);
    float3 trb = float3(brb.x - 0.1, tlf.y + 0.1, brb.z - 0.1);
    //TLF Corners    
    hitflag = rayBox(r.RayPos, r.RayDirection, tlf, tlb) || hitflag;
    hitflag = rayBox(r.RayPos, r.RayDirection, tlf, blf) || hitflag;
    hitflag = rayBox(r.RayPos, r.RayDirection, tlf, trf) || hitflag;
    //BRB

    hitflag = rayBox(r.RayPos, r.RayDirection, brb, blb) || hitflag;
    hitflag = rayBox(r.RayPos, r.RayDirection, brb, brf) || hitflag;
    hitflag = rayBox(r.RayPos, r.RayDirection, brb, trb) || hitflag;
    
    //BLF Corner
    float3 blf2 = float3(tlf.x + 0.1, brb.y - 0.1, tlf.z + 0.1);
    hitflag = rayBox(r.RayPos, r.RayDirection, blf, blb) || hitflag;
    hitflag = rayBox(r.RayPos, r.RayDirection, blf2, brf) || hitflag;
    
    //TRF Corner
    float3 trf2 = float3(brb.x + 0.1, tlf.y - 0.1, tlf.z + 0.1);
    hitflag = rayBox(r.RayPos, r.RayDirection, trf2, brf) || hitflag;
    hitflag = rayBox(r.RayPos, r.RayDirection, trf2, trb) || hitflag;
    
    //TLB Corner
    hitflag = rayBox(r.RayPos, r.RayDirection, tlb, trb) || hitflag;
    hitflag = rayBox(r.RayPos, r.RayDirection, tlb, blb) || hitflag;
    return hitflag;
}
bool RenderWireframeAtDepth(Ray r, StructuredBuffer<VoxelOctree> Octree)
{
    for (int iterations = 0; iterations < MAX_DEBUG_ITERATIONS; iterations++)
    {
        //if (Octree[iterations].Depth > padding)
        if (Octree[iterations].Depth == ViewDepth)
        {
            if (rayBox(r.RayPos, r.RayDirection, Octree[iterations].TopLeftFrontPosition, Octree[iterations].BottomRightBackPosition))
            {
                if (HitWireframe(r, Octree[iterations].TopLeftFrontPosition, Octree[iterations].BottomRightBackPosition))
                {
                    return true;
                }

            }
        }
    }
    return false;
    
}
bool RenderWireframeAboveDepth(Ray r, StructuredBuffer<VoxelOctree> Octree)
{
    for (int iterations = 0; iterations < MAX_DEBUG_ITERATIONS; iterations++)
    {
        //if (Octree[iterations].Depth > padding)
        if (Octree[iterations].Depth <= ViewDepth)
        {
            if (rayBox(r.RayPos, r.RayDirection, Octree[iterations].TopLeftFrontPosition, Octree[iterations].BottomRightBackPosition))
            {
                if (HitWireframe(r, Octree[iterations].TopLeftFrontPosition, Octree[iterations].BottomRightBackPosition))
                {
                    return true;
                }

            }
        }
    }
    return false;
    
}
bool RenderWireframeBox(Ray r, StructuredBuffer<VoxelOctree> Octree)
{
    for (int iterations = 0; iterations < MAX_DEBUG_ITERATIONS; iterations++)
    {
        //if (Octree[iterations].Depth > padding)
        if (Octree[iterations].RGB == ViewDepth)
        {
            if (rayBox(r.RayPos, r.RayDirection, Octree[iterations].TopLeftFrontPosition, Octree[iterations].BottomRightBackPosition))
            {
                if (HitWireframe(r, Octree[iterations].TopLeftFrontPosition, Octree[iterations].BottomRightBackPosition))
                {
                    return true;
                }

            }
        }
    }
    return false;
    
}
float3 HeatmapColor(int val, float minValue, float maxValue)
{
    if (val < 0)
    {
        return float3(0, 0, 0);

    }
    // Normalize the value within the range [0, 1]
    float t = saturate(float(val - minValue) / float(maxValue - minValue));
    
// Clamp the value between 0 and 1
    float value = saturate(t);

    // Define the colors for blue, green, yellow, and red
    float3 blueColor = float3(0, 0, 1);
    float3 greenColor = float3(0, 1, 0);
    float3 yellowColor = float3(1, 1, 0);
    float3 redColor = float3(1, 0, 0);

    float3 color;

    if (value < 0.3333f)
    {
        // Interpolate between blue and green for values < 0.3333
        color = lerp(blueColor, greenColor, value * 3.0f);
    }
    else if (value < 0.6666f)
    {
        // Interpolate between green and yellow for values >= 0.3333 and < 0.6666
        color = lerp(greenColor, yellowColor, (value - 0.3333f) * 3.0f);
    }
    else
    {
        // Interpolate between yellow and red for values >= 0.6666
        color = lerp(yellowColor, redColor, (value - 0.6666f) * 3.0f);
    }

    return color;
}
[numthreads(16, 16, 1)]
void main(int3 groupThreadID : SV_GroupThreadID,
	int3 dispatchThreadID : SV_DispatchThreadID)
{

    int x = dispatchThreadID.x;
    int y = dispatchThreadID.y;
    
    if (dispatchThreadID.y > 636)
    {
        return;
    }

    
    float3 camPos = cameraPosition.xyz;
    float2 res = float2(1184, 636);
    float fov = 1.0;
    float texx = float(x) / float(res.x);
    float texy = float(y) / float(res.y);
    float4 v = UVPositionCalculation(res, float2(texx, texy), projectionMatrix, fov);
    float3 rayVector = CalculateViewVector(v, viewMatrix, worldMatrix);
    //gOutput[int2(x, y)] = float4(rayVector.x, rayVector.y, rayVector.z, 1);
    
    Ray r;
    r.RayPos = camPos;
    r.RayDirection = rayVector;
    
    bool color = 0;
    switch (ViewMode)
    {
        case 0:
            if (heat)
            {
                gOutput[int2(x, y)] = float4(HeatmapColor((DoesRayIntersect(r, voxelOctree)), 0, MAX_ITERATIONS), 1);
                return;
            }
            color = (DoesRayIntersect(r, voxelOctree));
            break;

        case 1:
            color = (RenderBox(r, voxelOctree));
            break;
        case 2:
            color = (RenderAtDepth(r, voxelOctree));
            break;
        case 3:
            color = (RenderWireframeBox(r, voxelOctree));
            break;
        case 4:
            color = (RenderWireframeAtDepth(r, voxelOctree));
            break;
        case 5:
            color = (RenderWireframeAboveDepth(r, voxelOctree));
            break;
        default:
            color = (RenderWireframeAtDepth(r, voxelOctree));
            break;

    }

    if (color)
    {
        gOutput[int2(x, y)] = float4(1,1,1,1);
    }
    else
    {
        gOutput[int2(x, y)] = float4(rayVector, 1);
    }

}


//Indirection texture
//A(0,0) B(1,0)C(2,0)D(3,0)
//__________________________________
//| - - | - - | - - | - - |
//| - - | - - | - - | - - |
//__________________________________
