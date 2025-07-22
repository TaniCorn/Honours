#include "../HLSLI/Raycasting.hlsli"
#include "../HLSLI/VoxelColorHelper.hlsli"
#include "../HLSLI/Voxel.hlsli"


#define MAX_STACK_SIZE 40
#define MAX_ITERATIONS 150
#define MODELAMOUNTS 8
#define CHILDOCTANTAMOUNTS 8
#define WIREFRAME_WIDTH 0.3f

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
    int amountOfOctrees; // Currently used for padding
};

struct VoxelColor
{
    uint rgba[256];
};
cbuffer ColorBuffer : register(b3)
{
    VoxelColor cpal[MODELAMOUNTS];
};
// Input and output structures
Texture2D gInput : register(t0);
StructuredBuffer<VoxelOctree> voxelOctree[MODELAMOUNTS] : register(t1);
StructuredBuffer<VoxelColor> palette[MODELAMOUNTS] : register(t9);
RWTexture2D<float4> gOutput : register(u0);


////////////// The 3 functions below are used for different rendering nodes to render the octree in different ways, but fundamentally they function the same //////////////


/// <summary>
/// Checks if the ray intersects with the wireframe of the octree at a specific depth or above
/// Useful for rendering wireframes
/// </summary>
bool RayIntersectWireframe(Ray r, StructuredBuffer<VoxelOctree> Octree, float3 offset, bool equalDepth)
{
    // Traverse the octree to find the leaf node
    VoxelOctree currentNode = Octree[0];
    if (!rayBox(r.RayPos, r.RayDirection, currentNode.TopLeftFrontPosition + offset, currentNode.BottomRightBackPosition + offset))
    {
        return false; //No intersections with bounding box, Return invalid color index
    }
    int stackIndexes[MAX_STACK_SIZE];
    int stackTop = 0;
    stackIndexes[stackTop] = 0;
    stackTop++;
    for (int iterations = 0; iterations < MAX_ITERATIONS && stackTop > 0; iterations++)
    {
        //Get top of stack
        int OctreeStride = stackIndexes[--stackTop];
        currentNode = Octree[OctreeStride];
        
        if (currentNode.Depth <= ViewDepth)
        {
            if (equalDepth)
            {
                if (currentNode.Depth == ViewDepth)
                {
                    if (HitWireframe(r, currentNode.TopLeftFrontPosition + offset, currentNode.BottomRightBackPosition + offset, WIREFRAME_WIDTH))
                    {
                        return true;
                    }
                }
            }
            else
            {
                if (HitWireframe(r, currentNode.TopLeftFrontPosition + offset, currentNode.BottomRightBackPosition + offset, WIREFRAME_WIDTH))
                {
                    return true;
                }
            }

        }
        
        // If the node is not a leaf node, push its child nodes onto the stack
        for (int i = 0; i < CHILDOCTANTAMOUNTS; i++)
        {
            //Check all octant indexes
            int Stride = currentNode.Octants[i];
            if (Stride != 0)
            {
                //Grab child octant
                VoxelOctree childNode = Octree[Stride];
                if (childNode.Depth > ViewDepth)
                {
                    break;
                }
                //If we can hit the childoctant, add it to the stack
                if (rayBox(r.RayPos, r.RayDirection, childNode.TopLeftFrontPosition + offset, childNode.BottomRightBackPosition + offset))
                {
                    stackIndexes[stackTop] = Stride;
                    stackTop++;
                }
    

            }
        }
    }
    return false; // No intersection found, return invalid color index
}

/// <summary>
/// Checks if the ray intersects with the octree at a specific depth
/// Useful for rendering octants and heatmaps
/// </summary>
void RayIntersectAtDepth(Ray r, StructuredBuffer<VoxelOctree> Octree, float3 offset, out bool doesIntersect, out int octantStride, out int colorIndex, out int rayTravelAmount)
{
    doesIntersect = false;
    octantStride = -1; // Invalid stride
    colorIndex = 300; // Invalid color index
    rayTravelAmount = 0;
    // This function is just for testing purposes
    VoxelOctree currentNode = Octree[0];
    if (!rayBox(r.RayPos, r.RayDirection, currentNode.TopLeftFrontPosition + offset, currentNode.BottomRightBackPosition + offset))
    {
        return;
    }
    
    //Initialising stack
    int stackIndexes[MAX_STACK_SIZE];
    int stackTop = 0;
    stackIndexes[stackTop] = 0;
    stackTop++;
    
    for (int iterations = 0; iterations < MAX_ITERATIONS && stackTop > 0; iterations++)
    {
        int OctreeStride = stackIndexes[--stackTop];
        currentNode = Octree[OctreeStride];
        
        if (currentNode.Depth == ViewDepth)
        {
            doesIntersect = true;
            octantStride = OctreeStride;
            rayTravelAmount = iterations;
            colorIndex = currentNode.RGB; // Return the color index
            return;
        }
        
        float closest[CHILDOCTANTAMOUNTS];
        for (int i = 0; i < CHILDOCTANTAMOUNTS; i++)
        {
            closest[i] = 99999;
            //Check all octant indexes
            int Stride = currentNode.Octants[i];
            if (Stride != 0)
            {
                //Grab child octant
                VoxelOctree childNode = Octree[Stride];
                //If we can hit the childoctant, add it to the stack

                float3 tlf = childNode.TopLeftFrontPosition + offset;
                float3 brb = childNode.BottomRightBackPosition + offset;
                float3 loc = float3(brb.x - tlf.x, tlf.y - brb.y, brb.z - tlf.z) / 2.0f;
                loc = loc - r.RayPos;
                float3 t0 = (float3(tlf.x, brb.y, tlf.z) - r.RayPos) / r.RayDirection;
                float3 t1 = (float3(brb.x, tlf.y, brb.z) - r.RayPos) / r.RayDirection;

                float3 tMax = max(t0, t1);

                float tEnterMax = min(min(tMax.x, tMax.y), tMax.z);
                closest[i] = tEnterMax;
            }
        }
        
        float closestindex[CHILDOCTANTAMOUNTS];
        float close;
        for (int i = 0; i < CHILDOCTANTAMOUNTS; i++)
        {
            close = 99999;
            for (int j = 0; j < CHILDOCTANTAMOUNTS; j++)
            {
                if (closest[j] < close)
                {
                    close = closest[j];
                    closestindex[i] = j;
                }
            }
            closest[closestindex[i]] = 999999;
            
        }
        for (int i = CHILDOCTANTAMOUNTS - 1; i >= 0; i--)
        {
            
            int Stride = currentNode.Octants[closestindex[i]];

            
            if (Stride != 0)
            {
                VoxelOctree childNode = Octree[Stride];
                
                if (rayBox(r.RayPos, r.RayDirection, childNode.TopLeftFrontPosition + offset, childNode.BottomRightBackPosition + offset))
                {
                    stackIndexes[stackTop] = Stride;
                    stackTop++;
                }

            }
        }

    } // End of loop
    
    doesIntersect = false;
    octantStride = -1;
    rayTravelAmount = iterations;
    colorIndex = 300; // No intersection found, return invalid color index
}

/// <summary>
/// Checks if the ray intersects with a valid voxel in the octree
/// Useful for rendering the actual desired voxel models
/// </summary>
uint RayIntersectValidVoxel(Ray r, StructuredBuffer<VoxelOctree> Octree, float3 offset)
{
    // This function is just for testing purposes
    VoxelOctree currentNode = Octree[0];
    if (!rayBox(r.RayPos, r.RayDirection, currentNode.TopLeftFrontPosition + offset, currentNode.BottomRightBackPosition + offset))
    {
        return 300; //No intersections with bounding box, Return invalid color index
    }
    
    //Initialising stack
    int stackIndexes[MAX_STACK_SIZE];
    int stackTop = 0;
    stackIndexes[stackTop] = 0;
    stackTop++;
    
    for (int iterations = 0; iterations < MAX_ITERATIONS && stackTop > 0; iterations++)
    {
        int OctreeStride = stackIndexes[--stackTop];
        currentNode = Octree[OctreeStride];
        
        // Does ray intersect a valid voxel
        if (currentNode.RGB >= 0 && currentNode.RGB <= 256)
        {
            return currentNode.RGB;
        }
        
        float closest[CHILDOCTANTAMOUNTS];
        for (int i = 0; i < CHILDOCTANTAMOUNTS; i++)
        {
            closest[i] = 99999;
            //Check all octant indexes
            int Stride = currentNode.Octants[i];
            if (Stride != 0)
            {
                //Grab child octant
                VoxelOctree childNode = Octree[Stride];
                //If we can hit the childoctant, add it to the stack

                float3 tlf = childNode.TopLeftFrontPosition + offset;
                float3 brb = childNode.BottomRightBackPosition + offset;
                float3 loc = float3(brb.x - tlf.x, tlf.y - brb.y, brb.z - tlf.z) / 2.0f;
                loc = loc - r.RayPos;
                float3 t0 = (float3(tlf.x, brb.y, tlf.z) - r.RayPos) / r.RayDirection;
                float3 t1 = (float3(brb.x, tlf.y, brb.z) - r.RayPos) / r.RayDirection;

                float3 tMax = max(t0, t1);

                float tEnterMax = min(min(tMax.x, tMax.y), tMax.z);
                closest[i] = tEnterMax;
            }
        }
        
        float closestindex[CHILDOCTANTAMOUNTS];
        float close;
        for (int i = 0; i < CHILDOCTANTAMOUNTS; i++)
        {
            close = 99999;
            for (int j = 0; j < CHILDOCTANTAMOUNTS; j++)
            {
                if (closest[j] < close)
                {
                    close = closest[j];
                    closestindex[i] = j;
                }
            }
            closest[closestindex[i]] = 999999;
            
        }
        for (int i = CHILDOCTANTAMOUNTS - 1; i >= 0; i--)
        {
            
            int Stride = currentNode.Octants[closestindex[i]];

            
            if (Stride != 0)
            {
                VoxelOctree childNode = Octree[Stride];
                
                if (rayBox(r.RayPos, r.RayDirection, childNode.TopLeftFrontPosition + offset, childNode.BottomRightBackPosition + offset))
                {
                    stackIndexes[stackTop] = Stride;
                    stackTop++;
                }

            }
        }

    }

    return 300; // No intersection found, return invalid color index
}

    
[numthreads(16, 16, 1)]
void main(int3 groupThreadID : SV_GroupThreadID,
	int3 dispatchThreadID : SV_DispatchThreadID)
{
    int x = dispatchThreadID.x;
    int y = dispatchThreadID.y;
    
    // Quick Abort when reaching out of bounds for y pixel as the resolution in y is not nicely divisible by the 16 threads
    if (dispatchThreadID.y > 636)
    {
        return;
    }

    float3 camPos = cameraPosition.xyz;
    float2 res = float2(1184, 636);
    float fov = 1.0;
    
    // Calculating the texel coordinates based on resolution and dispatch thread ID
    float texx = float(x) / float(res.x);
    float texy = float(y) / float(res.y);
    
    // Creating a ray from the texel moving outwards from the camera position and fov amount in perspective view
    float4 v = UVPositionCalculation(res, float2(texx, texy), projectionMatrix, fov);
    float3 rayVector = CalculateViewVector(v, viewMatrix, worldMatrix);
    
    
    Ray ray;
    ray.RayPos = camPos;
    ray.RayDirection = rayVector;
    
    // Setting the output color to the ray vector, so that any missed rays will display the view vector direction
    float4 outputColor = float4(rayVector, 1);
    
    // Heatmap variables
    int heatIterations = 0;
    int heatHits = 0;
    
    // We loop 8 times to test hits with all 8 octrees in the array
    for (int i = 0; i < MODELAMOUNTS; i++)
    {
        // Get the voxel octree for the current index
        StructuredBuffer<VoxelOctree> vo = voxelOctree[i];
        StructuredBuffer<VoxelColor> cl = palette[i];
        
        // This is a hardcoded offset for each model
        float3 modelOffsets = float3((i % 4) * 200, saturate(i - 3) * 200, 0); //Model offsets
        
        // out variables for the test function
        uint colorIndex = 0;
        uint octantLocation = 0; // Redundant now
        bool doesIntersect = false;
        int modelHeatIterations = 0;
        
        
        switch (ViewMode)
        {
            case 0:
                // Basic ray intersection test, with color index later on
                colorIndex = RayIntersectValidVoxel(ray, vo, modelOffsets);
                if (colorIndex < 299)
                {
                    uint colPal = cl[0].rgba[colorIndex]; //Get RGBA from pallette of model and color index
                    outputColor = UnpackVoxelColor(colPal);
                }
                break;
            case 1:
                // Heatmap mode, additive
                RayIntersectAtDepth(ray, vo, modelOffsets, doesIntersect, octantLocation, colorIndex, modelHeatIterations);
                heatIterations += modelHeatIterations;
                outputColor = float4(HeatmapColor(heatIterations, 0, MAX_ITERATIONS), 1);
                break;
            case 2:
                // Heatmap mode, average // TODO: REMOVE
                RayIntersectAtDepth(ray, vo, modelOffsets, doesIntersect, octantLocation, colorIndex, modelHeatIterations);
                heatIterations += modelHeatIterations;
                outputColor = float4(HeatmapColor(heatIterations, 0, MAX_ITERATIONS), 1);
                break;
            case 3:
                // Render colored cubes at depth
                // Finds the specific octant that this ray will intersect
                // Use that octant to render the ray against the cube
                RayIntersectAtDepth(ray, vo, modelOffsets, doesIntersect, octantLocation, colorIndex, modelHeatIterations);
                if(colorIndex >= 500)
                    outputColor = GetColorFromOctant(colorIndex - 500);
                break;
            case 4:
                // Render wireframe at depth
                // Finds the specific octant that this ray will intersect
                // Use that octant to test against the wireframe
                colorIndex = RayIntersectWireframe(ray, vo, modelOffsets, true);
                if (colorIndex)
                    outputColor = float4(1, 1, 1, 1);
                break;
            case 5:
                // Render wireframes above depth
                colorIndex = RayIntersectWireframe(ray, vo, modelOffsets, false);
                if (colorIndex)
                    outputColor = float4(1, 1, 1, 1);
                break;
        }
        
        gOutput[int2(x, y)] = outputColor;
    }
}


//Indirection texture
//A(0,0) B(1,0)C(2,0)D(3,0)
//__________________________________
//| - - | - - | - - | - - |
//| - - | - - | - - | - - |
//__________________________________
