#include "../VoxelOctree.hlsli"
#define MIN_SIZE 1
#define MAX_DEPTH 500
static uint MaxStride = 0;

cbuffer VoxelModeDims : register(b0)
{
    int DimX;
    int DimY;
    int DimZ;
};


StructuredBuffer<Voxel> inputOctree : register(t0);
RWStructuredBuffer<VoxelOctree> outputOctree : register(u0);

bool inBoundary(float3 voxPoint, float3 topLeftFrontPoint, float3 bottomRightBackPoint)
{
    //AABB and point Collision check
    return (voxPoint.x >= topLeftFrontPoint.x && voxPoint.x <= bottomRightBackPoint.x &&
        voxPoint.y >= bottomRightBackPoint.y && voxPoint.y <= topLeftFrontPoint.y &&
        voxPoint.z >= topLeftFrontPoint.z && voxPoint.z <= bottomRightBackPoint.z);

}



int DetermineOctant(float3 voxPosition, float3 topLeftFrontPoint, float3 bottomRightBackPoint)
{
    float3 TLFBound = topLeftFrontPoint;
    float3 BRBBound = bottomRightBackPoint;
    float3 position = voxPosition;
    
     // If the point is out of bounds
    if (!inBoundary(voxPosition, TLFBound, BRBBound))
    {
        return -1;
    }

    float midx = ((TLFBound.x
                 + BRBBound.x) / 2.0f);
    float midy = ((TLFBound.y
                + BRBBound.y) / 2.0f);
    float midz = ((TLFBound.z
                + BRBBound.z) / 2.0f);

    if (position.z <= midz)
    {
        if (position.y > midy)
        {
            if (position.x <= midx)
            {
                return TLF;
            }
            else
            {
                return TRF;
            }
        }
        else
        {
            if (position.x <= midx)
            {
                return BLF;
            }
            else
            {
                return BRF;
            }
        }
    }
    else
    {
        if (position.y > midy)
        {
            if (position.x <= midx)
            {
                return TLB;
            }
            else
            {
                return TRB;
            }
                
        }
        else
        {
            if (position.x <= midx)
            {
                return BLB;
            }
            else
            {
                return BRB;
            }
        }
    }
}

float3 GetTLFBound(float3 TLFBound, float3 BRBBound, int OctantIndex)
{
    float midx = ((TLFBound.r
                 + BRBBound.r) / 2.0f);
    float midy = ((TLFBound.g
                + BRBBound.g) / 2.0f);
    float midz = ((TLFBound.b
                + BRBBound.b) / 2.0f);
    
    switch (OctantIndex)
    {
        case TLF:
            return TLFBound;
        case TRF:
            return float3(midx, TLFBound.g, TLFBound.b);
        case BLF:
            return float3(TLFBound.r, midy, TLFBound.b);
        case BRF:
            return float3(midx, midy, TLFBound.b);
        case TLB:
            return float3(TLFBound.r, TLFBound.g, midz);
        case TRB:
            return float3(midx, TLFBound.g, midz);
        case BLB:
            return float3(TLFBound.r, midy, midz);
        case BRB:
            return float3(midx, midy, midz);
        default:
            break;
    }
    return float3(0, 0, 0);

}


float3 GetBRBBound(float3 TLFBound, float3 BRBBound, int OctantIndex)
{
    float midx = ((TLFBound.r
                 + BRBBound.r) / 2.0f);
    float midy = ((TLFBound.g
                + BRBBound.g) / 2.0f);
    float midz = ((TLFBound.b
                + BRBBound.b) / 2.0f);
    
    switch (OctantIndex)
    {
        case TLF:
            return float3(midx, midy, midz);;
        case TRF:
            return float3(BRBBound.r, midy, midz);
        case BLF:
            return float3(midx, BRBBound.g, midz);
        case BRF:
            return float3(BRBBound.r, BRBBound.g, midz);
        case TLB:
            return float3(midx, midy, BRBBound.b);
        case TRB:
            return float3(BRBBound.r, midy, BRBBound.b);
        case BLB:
            return float3(midx, BRBBound.g, BRBBound.b);
        case BRB:
            return BRBBound;
        default:
            break;
    }
    return float3(0, 0, 0);

}
bool ShouldSubdivide(float3 TLFBound, float3 BRBBound, uint Depth)
{
    if (Depth >= MAX_DEPTH)
    {
        return false;
    }
    //Could replace with depth
    if (abs(TLFBound.r - BRBBound.r) <= MIN_SIZE
           && abs(TLFBound.g - BRBBound.g) <= MIN_SIZE
           && abs(TLFBound.b - BRBBound.b) <= MIN_SIZE)
    {
        return false;
    }
    return true;
}

//morton code xyz 0b 000 001 001 110 & 0b111 << layercoubt (3) -> 000 010 000 000 >> layercount * 3 -> 000 010

void InsertVoxel(VoxelOctree RootNode, Voxel Vox, RWStructuredBuffer<VoxelOctree> Octree)
{
    VoxelOctree currentNode = RootNode;
    uint Depth = 0;
    uint currentIndex = 0;
    // Traverse the octree to find the leaf node
    for (int i = 0; i < MAX_DEPTH; i++)
    {
        if (!ShouldSubdivide(currentNode.TopLeftFrontPosition, currentNode.BottomRightBackPosition, Depth))
        {
            Octree[currentIndex].RGB = 1;
            Octree[currentIndex].VoxelPosition = Vox.voxPosition;
            return;
        }
        
        // Determine the octant containing the voxel position
        int octantIndex = DetermineOctant(Vox.voxPosition, currentNode.TopLeftFrontPosition, currentNode.BottomRightBackPosition);
        //Octree[currentIndex].RGB = octantIndex; //DEBUGGING PURPOSES
        if (octantIndex < 0)//Outside of octant
        {
            return;
        }
        //If octant hasn't been created yet, allocate at the end of the buffer
            if (currentNode.Octants[octantIndex] == 0)
            {
                Depth++;
                MaxStride++;
            //currentNode.Octants[octantIndex] = MaxStride;
                Octree[currentIndex].Octants[octantIndex] = MaxStride;
            Octree[MaxStride].BottomRightBackPosition = GetBRBBound(currentNode.TopLeftFrontPosition, currentNode.BottomRightBackPosition, octantIndex);
            Octree[MaxStride].TopLeftFrontPosition = GetTLFBound(currentNode.TopLeftFrontPosition, currentNode.BottomRightBackPosition, octantIndex);
                Octree[MaxStride].Depth = Depth;
            }
        uint strideIndex = currentNode.Octants[octantIndex];
        currentIndex = strideIndex;
            // Move to the child node in the appropriate octant
        currentNode = Octree[strideIndex];
        Depth = currentNode.Depth;

    }
    
}

[numthreads(1, 1, 1)]
void main(int3 groupThreadID : SV_GroupThreadID,
	int3 dispatchThreadID : SV_DispatchThreadID)
{
    uint numVoxels = 0;
    uint numStride = 0;
    inputOctree.GetDimensions(numVoxels, numStride);
    if (dispatchThreadID.x > numVoxels)
    {
        return;
    }
    outputOctree[0].TopLeftFrontPosition = float3(0, DimY, 0);
    outputOctree[0].BottomRightBackPosition = float3(DimX, 0, DimZ);
    for (int i = 0; i < numVoxels; i++)
    {
        InsertVoxel(outputOctree[0], inputOctree[i], outputOctree);
        
    }
    //for (int i = 0; i < 9999; i++)
    //{
    //    outputOctree[i].RGB = i;
    //}

}