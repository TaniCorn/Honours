#include "../Resources/VoxelOctree.hlsli"
#include "../Resources/Raycasting.hlsli"
struct Ray
{
    float3 RayPos;
    float3 RayDirection;
};
#define MAX_STACK_SIZE 40
#define MAX_ITERATIONS 150

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
struct VoxelColor
{
    uint rgba[256];
};

// Input and output structures
Texture2D gInput : register(t0);
StructuredBuffer<VoxelOctree> voxelOctree[8] : register(t1);
StructuredBuffer<VoxelColor> palette[8] : register(t9);
RWTexture2D<float4> gOutput : register(u0);


float UnpackVoxelColor(uint color, int rgba)
{
    //Unpacks a uint color to individual channels rgba
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
}
bool HitWireframe(Ray r, float3 tlf, float3 brb, float width)
{
    bool hitflag = false;
    //Top
    float3 tlb = float3(tlf.x + width, tlf.y - width, brb.z + width);
    float3 blf = float3(tlf.x + width, brb.y + width, tlf.z + width);
    float3 trf = float3(brb.x - width, tlf.y - width, tlf.z + width);
    
    float3 blb = float3(tlf.x - width, brb.y - width, brb.z - width);
    float3 brf = float3(brb.x - width, brb.y + width, tlf.z - width);
    float3 trb = float3(brb.x - width, tlf.y + width, brb.z - width);
    //TLF Corners    
    hitflag = rayBox(r.RayPos, r.RayDirection, tlf, tlb) || hitflag;
    hitflag = rayBox(r.RayPos, r.RayDirection, tlf, blf) || hitflag;
    hitflag = rayBox(r.RayPos, r.RayDirection, tlf, trf) || hitflag;
    //BRB

    hitflag = rayBox(r.RayPos, r.RayDirection, brb, blb) || hitflag;
    hitflag = rayBox(r.RayPos, r.RayDirection, brb, brf) || hitflag;
    hitflag = rayBox(r.RayPos, r.RayDirection, brb, trb) || hitflag;
    
    //BLF Corner
    float3 blf2 = float3(tlf.x + width, brb.y - width, tlf.z + width);
    hitflag = rayBox(r.RayPos, r.RayDirection, blf, blb) || hitflag;
    hitflag = rayBox(r.RayPos, r.RayDirection, blf2, brf) || hitflag;
    
    //TRF Corner
    float3 trf2 = float3(brb.x + width, tlf.y - width, tlf.z + width);
    hitflag = rayBox(r.RayPos, r.RayDirection, trf2, brf) || hitflag;
    hitflag = rayBox(r.RayPos, r.RayDirection, trf2, trb) || hitflag;
    
    //TLB Corner
    hitflag = rayBox(r.RayPos, r.RayDirection, tlb, trb) || hitflag;
    hitflag = rayBox(r.RayPos, r.RayDirection, tlb, blb) || hitflag;
    return hitflag;
}
bool VoxelDoesRayIntersectAboveDepth(Ray r, StructuredBuffer<VoxelOctree> Octree, float3 offset)
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
            if (HitWireframe(r, currentNode.TopLeftFrontPosition + offset, currentNode.BottomRightBackPosition + offset, 0.05))
            {
                return true;
            }
        }
        
        // If the node is not a leaf node, push its child nodes onto the stack
        for (int i = 0; i < 8; i++)
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
int VoxelDoesRayIntersect(Ray r, StructuredBuffer<VoxelOctree> Octree, float3 offset)
{
    // Traverse the octree to find the leaf node
    VoxelOctree currentNode = Octree[0];
    if (!rayBox(r.RayPos, r.RayDirection, currentNode.TopLeftFrontPosition + offset, currentNode.BottomRightBackPosition + offset))
    {
        return -1;
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
        //if (!rayBox(r.RayPos, r.RayDirection, currentNode.TopLeftFrontPosition + offset, currentNode.BottomRightBackPosition + offset))
        //{
        //    continue;
        //}
            
        //if (currentNode.RGB >= 0 && currentNode.RGB <= 256)
        if (currentNode.Depth == ViewDepth)
        {
            return OctreeStride;
        }
        
        float closest[8];
        for (int i = 0; i < 8; i++)
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
        
        float closestindex[8];
        float close;
        for (int i = 0; i < 8; i++)
        {
            close = 99999;
            for (int j = 0; j < 8; j++)
            {
                if (closest[j] < close)
                {
                    close = closest[j];
                    closestindex[i] = j;
                }
            }
            closest[closestindex[i]] = 999999;
            
        }
        
        
        for (int i = 7; i >= 0; i--)
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
    return -1;
}
uint DoesRayIntersect(Ray r, StructuredBuffer<VoxelOctree> Octree, float3 offset)
{
    // Traverse the octree to find the leaf node
    VoxelOctree currentNode = Octree[0];
    if (!rayBox(r.RayPos, r.RayDirection, currentNode.TopLeftFrontPosition + offset, currentNode.BottomRightBackPosition + offset))
    {
        if (heat)
        {
            return 0;
        }
        else
        {
            return 300; //No intersections with bounding box, Return invalid color index
            
        }
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
        //if (!rayBox(r.RayPos, r.RayDirection, currentNode.TopLeftFrontPosition + offset, currentNode.BottomRightBackPosition + offset))
        //{
        //    continue;
        //}
            
        if (currentNode.RGB >= 0 && currentNode.RGB <= 256 && !heat)
        {
                return currentNode.RGB;
        }
        if (currentNode.Depth == ViewDepth && heat)
        {
                return iterations;
        }
        
        float closest[8];
        for (int i = 0; i < 8; i++)
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
        
        float closestindex[8];
        float close;
        for (int i = 0; i < 8; i++)
        {
            close = 99999;
            for (int j = 0; j < 8; j++)
            {
                if (closest[j] < close)
                {
                    close = closest[j];
                    closestindex[i] = j;
                }
            }
            closest[closestindex[i]] = 999999;
            
        }
        
        
        for (int i = 7; i >= 0; i--)
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
    if (heat)
    {
        return iterations;
    }
    else
    {
        return 300; //No intersections with bounding box, Return invalid color index
            
    }
}
uint HeatDoesRayIntersect(Ray r, StructuredBuffer<VoxelOctree> Octree, float3 offset)
{
    // Traverse the octree to find the leaf node
    VoxelOctree currentNode = Octree[0];
    if (!rayBox(r.RayPos, r.RayDirection, currentNode.TopLeftFrontPosition + offset, currentNode.BottomRightBackPosition + offset))
    {
       return 0; // No intersection found
    }
    
    //Initialising stack
    int stackIndexes[MAX_STACK_SIZE];
    int stackTop = 0;
    stackIndexes[stackTop] = 0;
    stackTop++;
    
    
    for (int iterations = 0; iterations < MAX_ITERATIONS && stackTop > 0; iterations++)
    {
        //Get top of stack
        int OctreeStride = stackIndexes[--stackTop];
        currentNode = Octree[OctreeStride];
    
        if (currentNode.Depth == ViewDepth)
        {
           return iterations;//Intersection found
        }
        
        // If the node is not a leaf node, push its child nodes onto the stack
        for (int i = 0; i < 8; i++)
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
    return iterations; // No intersection found
}

int RenderBoxAtDepth(Ray r, StructuredBuffer<VoxelOctree> Octree, float3 offset, uint stride)
{
    //Renders a box at depth
    VoxelOctree currentNode = Octree[stride];
    if (rayBox(r.RayPos, r.RayDirection, currentNode.TopLeftFrontPosition + offset, currentNode.BottomRightBackPosition + offset))
    {
        return currentNode.RGB;
    }
    return 0;
}


bool RenderWireframeAtDepth(Ray r, StructuredBuffer<VoxelOctree> Octree, float3 offset, uint stride)
{
    //Renders octree wireframe at depths
    if (rayBox(r.RayPos, r.RayDirection, Octree[stride].TopLeftFrontPosition + offset, Octree[stride].BottomRightBackPosition + offset))
    {
        if (HitWireframe(r, Octree[stride].TopLeftFrontPosition + offset, Octree[stride].BottomRightBackPosition + offset, 0.2f))
        {
            return true;
        }
    }
    return false;
}
    
bool RenderWireframeAboveDepth(Ray r, StructuredBuffer<VoxelOctree> Octree, float3 offset)
{
    //Renders octree wireframe above depths
    return VoxelDoesRayIntersectAboveDepth(r, Octree, offset);

}

float3 HeatmapColor(int val, float minValue, float maxValue)
{
    if (val < 0)
    {
        return float3(0, 0, 1);

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

    float3 outputColor;

    if (value < 0.3333f)
    {
        // Interpolate between blue and green for values < 0.3333
        outputColor = lerp(blueColor, greenColor, value * 3.0f);
    }
    else if (value < 0.6666f)
    {
        // Interpolate between green and yellow for values >= 0.3333 and < 0.6666
        outputColor = lerp(greenColor, yellowColor, (value - 0.3333f) * 3.0f);
    }
    else if(value < 0.98000f)
    {
        // Interpolate between yellow and red for values >= 0.6666
        outputColor = lerp(yellowColor, redColor, (value - 0.6666f) * 3.0f);
    }
    else
    {
        outputColor = float3(0, 0, 0);

    }

    return outputColor;
}

float4 GetColorFromOctant(int index)
{
    //Each color corresponds with RGB and XYZ
    switch (index)
    {
        case TLF:
            return float4(0, 1, 0, 1);
        case TRF:
            return float4(1, 1, 0, 1);
        case BLF:
            return float4(0, 0, 0, 1);
        case BRF:
            return float4(1, 0, 0, 1);
        case TLB:
            return float4(0, 1, 1, 1);
        case TRB:
            return float4(1, 1, 1, 1);
        case BLB:
            return float4(0, 0, 1, 1);
        case BRB:
            return float4(1, 0, 1, 1);
        default:
            return float4(0, 0, 0, 0);
    }

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
    
    Ray ray;
    ray.RayPos = camPos;
    ray.RayDirection = rayVector;
    
    gOutput[int2(x, y)] = float4(rayVector, 1);
    
    int heatIterations = 0;
    int heatHits = 0;
    for (int i = 0; i < 8; i++)
    {
        StructuredBuffer<VoxelOctree> vo = voxelOctree[i];
        
        float3 offset = float3((i % 4) * 200, saturate(i - 3) * 200, 0);//Model offsets
        uint color = 0;
        
        //For Render Box and Wireframe only
        uint stride = 0;
        if (ViewMode == 3 || ViewMode == 4)
        {
            stride = VoxelDoesRayIntersect(ray, vo, offset);
        }
        
        
       switch (ViewMode)
        {
            case 0:
                color = DoesRayIntersect(ray, vo, offset);
                break;
            case 1:
                heatIterations += DoesRayIntersect(ray, vo, offset);
                break;
            case 2:
                int heat = HeatDoesRayIntersect(ray, vo, offset);
                if (heat > 0)
                {
                    heatHits++;
                }
                heatIterations += heat;
                break;
            case 3:
                color = RenderBoxAtDepth(ray, vo, offset, stride);
                break;
            case 4:
                color = RenderWireframeAtDepth(ray, vo, offset, stride);
                break;
            case 5:
                color = VoxelDoesRayIntersectAboveDepth(ray, vo, offset);
                break;
        }

        //For case 0
        if (ViewMode == 0 && color < 299)
        {
            StructuredBuffer<VoxelColor> cl = palette[0];
            uint colPal = cl[i].rgba[color];//Get RGBA from pallette of model and color index
            
            float r, g, b, a;
            r = UnpackVoxelColor(colPal, 0);
            g = UnpackVoxelColor(colPal, 1);
            b = UnpackVoxelColor(colPal, 2);
            a = UnpackVoxelColor(colPal, 3);
            gOutput[int2(x, y)] = float4(r, g, b, a);
        }
        else if(ViewMode > 0)
        {
            if (color >= 500)//Box render for colors
            {
                gOutput[int2(x, y)] = GetColorFromOctant(color - 500);
            }
            else if(color)//Other renders
            {
                gOutput[int2(x, y)] = float4(1, 1, 1, 1);
                
            }
        }
  
    }
    if (heat)
    {
        if (ViewMode == 1)//Additive hits
        {
            gOutput[int2(x, y)] = float4(HeatmapColor(heatIterations, 0, MAX_ITERATIONS), 1);
        }
        else
        {
            //Average hits
            if (heatHits > 0)
            {
                heatIterations /= heatHits;
                gOutput[int2(x, y)] = float4(HeatmapColor(heatIterations, 0, MAX_ITERATIONS), 1);
            }
            else
            {
                gOutput[int2(x, y)] = float4(HeatmapColor(heatIterations, 0, MAX_ITERATIONS), 1);
            }
        }
    }

}


//Indirection texture
//A(0,0) B(1,0)C(2,0)D(3,0)
//__________________________________
//| - - | - - | - - | - - |
//| - - | - - | - - | - - |
//__________________________________
