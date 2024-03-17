#include "../VoxelOctree.hlsli"
struct Ray
{
    float3 RayPos;
    float3 RayDirection;
};
#define MAX_STACK_SIZE 100
#define MAX_ITERATIONS 75
#define MAX_DEBUG_ITERATIONS 999

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
    int amountOfOctrees;
};
float UnpackVoxelColor(uint color, int rgba)
{
    const float coefficient = 255.f;
    switch (rgba)
    {
        case 0:
            //return float((color & 0x000000ff)) * coefficient;
            return float((color & 0x000000ff)) / coefficient;
        case 1:
            //return float((color & 0x0000ff00)) * coefficient;
            return float(((color >> 8) & 0x000000ff)) / coefficient;
        case 2:
            //return float((color & 0x00ff0000)) * coefficient;
            return float(((color >> 16) & 0x000000ff)) / coefficient;
        case 3:
            //return float((color & 0xff000000)) * coefficient;
            return 1.0f;
            return float(((color >> 24) & 0x000000ff)) / coefficient;
        default:
            return 1;
    }
    //

}

struct VoxelColor
{
    uint rgba[256];
};
#include "../GridShading.hlsli"

// Input and output structures
Texture2D gInput : register(t0);
StructuredBuffer<VoxelOctree> voxelOctree[8] : register(t1);
StructuredBuffer<VoxelColor> palette[8] : register(t9);
RWTexture2D<float4> gOutput : register(u0);

uint DoesRayIntersect(Ray r, StructuredBuffer<VoxelOctree> Octree, float offset)
{
    // Traverse the octree to find the leaf node
    VoxelOctree currentNode = Octree[0];
    if (!rayBox(r.RayPos, r.RayDirection, currentNode.TopLeftFrontPosition + offset, currentNode.BottomRightBackPosition + offset))
    {
        if (heat)
        {
            return 0; // Intersection found
        }
        else
        {
            return 300;
        }
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
            if (heat)
            {
                return iterations; // Intersection found
            }
            else
            {
                return currentNode.RGB;
            }
                
            
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
                if (rayBox(r.RayPos, r.RayDirection, childNode.TopLeftFrontPosition + offset, childNode.BottomRightBackPosition + offset))
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
            return MAX_ITERATIONS;
        }
        else
        {
           return iterations; // No intersection found//Hit bounding octree
        }
    }
    else
    {
        return 300; // No intersection found//Hit bounding octree
    }
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
    
    Ray ray;
    ray.RayPos = camPos;
    ray.RayDirection = rayVector;
    gOutput[int2(x, y)] = float4(rayVector, 1);
    int heatIterations = 0;
    //uint colPal = palette[4].rgba[45];
    //colPal = 4294967295;
    //colPal = 4456292;
    ////colPal = 0x00ffffe6;
    ////colPal = 1;
    //float r1, g, b, a;
    //r1 = UnpackVoxelColor(colPal, 0);
    //g = UnpackVoxelColor(colPal, 1);
    //b = UnpackVoxelColor(colPal, 2);
    //a = UnpackVoxelColor(colPal, 3);
    //gOutput[int2(x, y)] = float4(r1, g, b, a);
    //return;
    
    for (int i = 0; i < 8; i++)
    {
        StructuredBuffer<VoxelOctree> vo = voxelOctree[i];
        int color = 0;
        uint colorback = 0;
        switch (ViewMode)
        {
            case 0:
                if (heat)
                {
                    heatIterations += DoesRayIntersect(ray, vo, (i - 4) * 50);
                }
                colorback = (DoesRayIntersect(ray, vo, (i - 4) * 50));
                break;
            case 1:
                color = (RenderBox(ray, vo));
                break;
            case 2:
                color = (RenderAtDepth(ray, vo));
                break;
            case 3:
                color = (RenderWireframeBox(ray, vo));
                break;
            case 4:
                color = (RenderWireframeAtDepth(ray, vo));
                break;
            case 5:
                color = (RenderWireframeAboveDepth(ray, vo));
                break;
            default:
                color = (RenderWireframeAtDepth(ray, vo));
                break;

        }

        if (colorback < 299)
        {
            //float evalColor = float(color) / 255.0f;
            
            //gOutput[int2(x, y)] = float4(r, r, r, r);
            //gOutput[int2(x, y)] = float4(g,g,g,g);
            //gOutput[int2(x, y)] = float4(b,b,b,b);
            //gOutput[int2(x, y)] = float4(a,a,a,a);
            //gOutput[int2(x, y)] = float4(evalColor, evalColor, evalColor, 1);
            //gOutput[int2(x, y)] = float4(r,g,b,1);
                StructuredBuffer<VoxelColor> cl = palette[0];
                
                uint colPal = cl[i].rgba[colorback];
                //colPal = 0x58bc4c;
                float r, g, b, a;
                r = UnpackVoxelColor(colPal, 0);
                g = UnpackVoxelColor(colPal, 1);
                b = UnpackVoxelColor(colPal, 2);
                a = UnpackVoxelColor(colPal, 3);
                //float he = 1.f / 255.0f;
                //float c = 255.0f * he;
                //gOutput[int2(x, y)] = float4(c,c,c,c);
                gOutput[int2(x, y)] = float4(r, g, b, a);
            //gOutput[int2(x, y)] = float4(HeatmapColor(color, 0, 255), 1);
            //gOutput[int2(x, y)] = float4(1, 1, 1, 1);
        }
    }
    if (heat)
    {
        gOutput[int2(x, y)] = float4(HeatmapColor(heatIterations, 0, MAX_ITERATIONS), 1);
        
    }

}


//Indirection texture
//A(0,0) B(1,0)C(2,0)D(3,0)
//__________________________________
//| - - | - - | - - | - - |
//| - - | - - | - - | - - |
//__________________________________
