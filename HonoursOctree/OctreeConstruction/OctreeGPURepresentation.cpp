#include "OctreeGPURepresentation.h"
#define MAX_DEPTH 20
#define MIN_SIZE 1
#define TLF 0
#define TRF 1
#define BLF 2
#define BRF 3
#define TLB 4
#define TRB 5
#define BLB 6
#define BRB 7
OctreeGPURepresentation::OctreeGPURepresentation()
{
    for (int i = 0; i < 8; i++)
    {
        octantsStride[i] = 0;
    }
    colorIndex = 300;
    depth = 0;
}
OctreeGPURepresentation::~OctreeGPURepresentation()
{
}
void GPUOctree::InsertVoxel(Voxel* Vox)
{
    OctreeGPURepresentation* currentNode = &octree[0];
    UINT32 depth = 0;
    UINT32 currentIndex = 0;
    // Traverse the octree to find the leaf node
    for (int i = 0; i < MAX_DEPTH; i++)
    {
        if (!currentNode->shouldSubdivide())
        {
            currentNode->colorIndex = Vox->color;
            currentNode->voxelPosition = Vox->point;
            return;
        }

        // Determine the octant containing the voxel position
        int octantIndex = currentNode->determineOctant(Vox->point);
        //Octree[currentIndex].RGB = octantIndex; //DEBUGGING PURPOSES
        if (octantIndex < 0)//Outside of octant
        {
            return;
        }
        //If octant hasn't been created yet, allocate at the end of the buffer
        if (currentNode->octantsStride[octantIndex] == 0)
        {
            depth++;
            maxStride++;
            //currentNode.Octants[octantIndex] = MaxStride;
            currentNode->octantsStride[octantIndex] = maxStride;
            octree[maxStride].BottomRightBackPosition = currentNode->getChildBRBBound(octantIndex);
            octree[maxStride].TopLeftFrontPosition = currentNode->getChildTLFBound(octantIndex);
            octree[maxStride].depth = depth;
        }
        UINT32 strideIndex = currentNode->octantsStride[octantIndex];
        currentIndex = strideIndex;
        // Move to the child node in the appropriate octant
        currentNode = &octree[currentIndex];
        depth = currentNode->depth;

    }
}

void GPUOctree::Clear()
{
    for (int i = 0; i < maxStride; i++)
    {
        octree[i].colorIndex = 300;
        for (int j = 0; j < 8; j++)
        {
            octree[i].octantsStride[j] = 0;
        }
        octree[i].depth = 0;
    }
    maxStride = 0;
    for (auto vox : voxels)
    {
        delete vox;
    }
    voxels.clear();
}

bool OctreeGPURepresentation::shouldSubdivide() const
{
    if (depth >= MAX_DEPTH)
    {
        return false;
    }
    //Could replace with depth
    if (abs(TopLeftFrontPosition.x - BottomRightBackPosition.x) <= MIN_SIZE
        && abs(TopLeftFrontPosition.y - BottomRightBackPosition.y) <= MIN_SIZE
        && abs(TopLeftFrontPosition.z - BottomRightBackPosition.z) <= MIN_SIZE)
    {
        return false;
    }
    return true;
}
bool OctreeGPURepresentation::inBoundary(const XMFLOAT3 position) const
{
    //AABB and point Collision check
    return (position.x >= TopLeftFrontPosition.x && position.x <= BottomRightBackPosition.x &&
        position.y >= BottomRightBackPosition.y && position.y <= TopLeftFrontPosition.y &&
        position.z >= TopLeftFrontPosition.z && position.z <= BottomRightBackPosition.z);
}

XMFLOAT3 OctreeGPURepresentation::getChildTLFBound(const UINT32 octant) const
{
    XMFLOAT3 TLFBound = TopLeftFrontPosition;
    XMFLOAT3 BRBBound = BottomRightBackPosition;

    float midx = ((TLFBound.x
        + BRBBound.x) / 2.0f);
    float midy = ((TLFBound.y
        + BRBBound.y) / 2.0f);
    float midz = ((TLFBound.z
        + BRBBound.z) / 2.0f);

    switch (octant)
    {
    case TLF:
        return TLFBound;
    case TRF:
        return XMFLOAT3(midx, TLFBound.y, TLFBound.z);
    case BLF:
        return XMFLOAT3(TLFBound.x, midy, TLFBound.z);
    case BRF:
        return XMFLOAT3(midx, midy, TLFBound.z);
    case TLB:
        return XMFLOAT3(TLFBound.x, TLFBound.y, midz);
    case TRB:
        return XMFLOAT3(midx, TLFBound.y, midz);
    case BLB:
        return XMFLOAT3(TLFBound.x, midy, midz);
    case BRB:
        return XMFLOAT3(midx, midy, midz);
    default:
        break;
    }
    return XMFLOAT3(0, 0, 0);

}
XMFLOAT3 OctreeGPURepresentation::getChildBRBBound(const UINT32 octant) const
{
    XMFLOAT3 TLFBound = TopLeftFrontPosition;
    XMFLOAT3 BRBBound = BottomRightBackPosition;

    float midx = ((TLFBound.x
        + BRBBound.x) / 2.0f);
    float midy = ((TLFBound.y
        + BRBBound.y) / 2.0f);
    float midz = ((TLFBound.z
        + BRBBound.z) / 2.0f);

    switch (octant)
    {
    case TLF:
        return XMFLOAT3(midx, midy, midz);;
    case TRF:
        return XMFLOAT3(BRBBound.x, midy, midz);
    case BLF:
        return XMFLOAT3(midx, BRBBound.y, midz);
    case BRF:
        return XMFLOAT3(BRBBound.x, BRBBound.y, midz);
    case TLB:
        return XMFLOAT3(midx, midy, BRBBound.z);
    case TRB:
        return XMFLOAT3(BRBBound.x, midy, BRBBound.z);
    case BLB:
        return XMFLOAT3(midx, BRBBound.y, BRBBound.z);
    case BRB:
        return BRBBound;
    default:
        break;
    }
    return XMFLOAT3(0, 0, 0);
}
int OctreeGPURepresentation::determineOctant(XMFLOAT3 position) const
{
        XMFLOAT3 TLFBound = TopLeftFrontPosition;
        XMFLOAT3 BRBBound = BottomRightBackPosition;

        // If the point is out of bounds
        if (!inBoundary(position))
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

#undef TLF 0
#undef TRF 1
#undef BLF 2
#undef BRF 3
#undef TLB 4
#undef TRB 5
#undef BLB 6
#undef BRB 7
#undef MAX_DEPTH
#undef MIN_SIZE

