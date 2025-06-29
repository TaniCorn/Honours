#include "SVOConstructorCPU.h"
using namespace OctreeGPU;

#define MAX_DEPTH 20 // MAX DEPTH has to be defined as the GPU hlsl shader requires an immutable amount
#define TLF 0
#define TRF 1
#define BLF 2
#define BRF 3
#define TLB 4
#define TRB 5
#define BLB 6
#define BRB 7

OctreeGPU::SVOConstructorCPU::SVOConstructorCPU(int amountOfVoxels, UINT minVoxelSize) : 
    MinSize(minVoxelSize), MaxStride(0)
{
    Octree = std::make_unique<OctreeGPURepresentation[]>(amountOfVoxels * MAX_DEPTH);
    Voxels = std::make_unique<std::vector<Voxel>>(amountOfVoxels);
}

void SVOConstructorCPU::InsertVoxel(Voxel Vox)
{
    //Depth first insertion
    OctreeGPURepresentation* currentNode = &Octree[0];
    UINT32 Depth = 0;
    UINT32 currentIndex = 0;

	// Traverses and populates Octants in the Octree until it finds 
    //      a node that should not subdivide or reaches the maximum depth which is the leaf node.
    for (int i = 0; i < MAX_DEPTH; i++)
    {
        if (!currentNode->ShouldSubdivide(MinSize))
        {
            currentNode->ColorIndex = Vox.Color;
            currentNode->VoxelPosition = Vox.Point;
			Voxels->push_back(Vox);
            return;
        }

        // Determine which child octant should contain the voxel
        int octantIndex = currentNode->DetermineOctant(Vox.Point);
        // Voxel point was outside of the bounds of the current octant, and cannot insert
        if (octantIndex < 0)
        {
            return;
        }

		//If octant hasn't been created yet, allocate at the end of the Octree buffer via MaxStride
        if (currentNode->ChildOctantStride[octantIndex] == 0)
        {
            Depth++;
            //Child Octant will Point to location of an existing octant in the Octree using MaxStride as the end of the current buffer
            MaxStride++;
            currentNode->ChildOctantStride[octantIndex] = MaxStride;

			// Instead of actually creating a new octant, we use the next available index in the Octree array
            // Calculating the new octants bounds using the current nodes bounds and calculate the child octant bound
            Octree[MaxStride].BottomRightBackPosition = currentNode->GetChildBRBBound(octantIndex);
            Octree[MaxStride].TopLeftFrontPosition = currentNode->GetChildTLFBound(octantIndex);
            Octree[MaxStride].Depth = Depth;
            Octree[MaxStride].ColorIndex = 500 + octantIndex;
        }
        // Move the current node to search to the next child octant so we can continue searching for insertion point
        currentIndex = currentNode->ChildOctantStride[octantIndex];
        currentNode = &Octree[currentIndex];

        Depth = currentNode->Depth;

    }
}

const OctreeGPURepresentation& OctreeGPU::SVOConstructorCPU::GetOctant(const UINT32& Stride) const
{
	return Octree[Stride];
}

void SVOConstructorCPU::Clear()
{
    for (int i = 0; i < MaxStride; i++)
    {
        Octree[i].ColorIndex = 300;
        for (int j = 0; j < 8; j++)
        {
            Octree[i].ChildOctantStride[j] = 0;
        }
        Octree[i].Depth = 0;
    }
    MaxStride = 0;

    Voxels->clear();
}

OctreeGPURepresentation::OctreeGPURepresentation() : 
	TopLeftFrontPosition(XMFLOAT3(0, 0, 0)), BottomRightBackPosition(XMFLOAT3(0, 0, 0)), VoxelPosition(XMFLOAT3(0, 0, 0)),
	ColorIndex(300), Depth(0)
{
    for (int i = 0; i < 8; i++)
    {
        ChildOctantStride[i] = 0;
    }
}

bool OctreeGPURepresentation::ShouldSubdivide(int MinSize) const
{
    //Will stop subdividing based on size of voxel or Depth of Octree
    if (Depth >= MAX_DEPTH)
    {
        return false;
    }
    // Compares if the real size of the octant is smaller than the minimum size.
	// Is so, that means that the voxel size is larger than the octant size, and we should not subdivide.
    if (abs(TopLeftFrontPosition.x - BottomRightBackPosition.x) <= MinSize
        && abs(TopLeftFrontPosition.y - BottomRightBackPosition.y) <= MinSize
        && abs(TopLeftFrontPosition.z - BottomRightBackPosition.z) <= MinSize)
    {
        return false;
    }
    return true;
}
bool OctreeGPURepresentation::IsInBoundary(const XMFLOAT3 Position) const
{
    //AABB and Point Collision check
    return (Position.x >= TopLeftFrontPosition.x && Position.x <= BottomRightBackPosition.x &&
        Position.y >= BottomRightBackPosition.y && Position.y <= TopLeftFrontPosition.y &&
        Position.z >= TopLeftFrontPosition.z && Position.z <= BottomRightBackPosition.z);
}

XMFLOAT3 OctreeGPURepresentation::GetChildTLFBound(const UINT32 octant) const
{
    //Returns the new TLF coordinate of OctantIndex octant
    XMFLOAT3 TLFBound = TopLeftFrontPosition;
    XMFLOAT3 BRBBound = BottomRightBackPosition;

    float MidX = ((TLFBound.x
        + BRBBound.x) / 2.0f);
    float MidY = ((TLFBound.y
        + BRBBound.y) / 2.0f);
    float MidZ = ((TLFBound.z
        + BRBBound.z) / 2.0f);

    // Using the information of the parents TLF, BRB and midpoint coordinates,
	// We can return the new TLF coordinate of the child octant as it is relative to the parent octant.
    switch (octant)
    {
    case TLF:
        return TLFBound;
    case TRF:
        return XMFLOAT3(MidX, TLFBound.y, TLFBound.z);
    case BLF:
        return XMFLOAT3(TLFBound.x, MidY, TLFBound.z);
    case BRF:
        return XMFLOAT3(MidX, MidY, TLFBound.z);
    case TLB:
        return XMFLOAT3(TLFBound.x, TLFBound.y, MidZ);
    case TRB:
        return XMFLOAT3(MidX, TLFBound.y, MidZ);
    case BLB:
        return XMFLOAT3(TLFBound.x, MidY, MidZ);
    case BRB:
        return XMFLOAT3(MidX, MidY, MidZ);
    default:
        break;
    }
    return XMFLOAT3(0, 0, 0);

}
XMFLOAT3 OctreeGPURepresentation::GetChildBRBBound(const UINT32 octant) const
{
    //Returns the new BRB coordinate of OctantIndex octant
    XMFLOAT3 TLFBound = TopLeftFrontPosition;
    XMFLOAT3 BRBBound = BottomRightBackPosition;

    float MidX = ((TLFBound.x
        + BRBBound.x) / 2.0f);
    float MidY = ((TLFBound.y
        + BRBBound.y) / 2.0f);
    float MidZ = ((TLFBound.z
        + BRBBound.z) / 2.0f);

    // Using the information of the parents TLF, BRB and midpoint coordinates,
    // We can return the new BRB coordinate of the child octant as it is relative to the parent octant.
    switch (octant)
    {
    case TLF:
        return XMFLOAT3(MidX, MidY, MidZ);;
    case TRF:
        return XMFLOAT3(BRBBound.x, MidY, MidZ);
    case BLF:
        return XMFLOAT3(MidX, BRBBound.y, MidZ);
    case BRF:
        return XMFLOAT3(BRBBound.x, BRBBound.y, MidZ);
    case TLB:
        return XMFLOAT3(MidX, MidY, BRBBound.z);
    case TRB:
        return XMFLOAT3(BRBBound.x, MidY, BRBBound.z);
    case BLB:
        return XMFLOAT3(MidX, BRBBound.y, BRBBound.z);
    case BRB:
        return BRBBound;
    default:
        break;
    }
    return XMFLOAT3(0, 0, 0);
}
int OctreeGPURepresentation::DetermineOctant(XMFLOAT3 Position) const
{
    if (!IsInBoundary(Position))
    {
        return -1;
    }

    XMFLOAT3 TLFBound = TopLeftFrontPosition;
    XMFLOAT3 BRBBound = BottomRightBackPosition;

    float MidX = ((TLFBound.x
        + BRBBound.x) / 2.0f);
    float MidY = ((TLFBound.y
        + BRBBound.y) / 2.0f);
    float MidZ = ((TLFBound.z
        + BRBBound.z) / 2.0f);
	// We compare the position of the point with the midpoints of the octant to determine which child octant it belongs to
    // Remember the coordinate system (see .h)

	// I have no idea what this comment means, but it is from the original code
    //T = TLF ,Y, BRB ,mY,
    //B = TLF ,mY, BRB ,Y,
    //R = TLF mX,, BRB X,,
    //L = TLF X,, BRB mX,,
    if (Position.z <= MidZ)
    {
        if (Position.y > MidY)
        {
            if (Position.x <= MidX)
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
            if (Position.x <= MidX)
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
        if (Position.y > MidY)
        {
            if (Position.x <= MidX)
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
            if (Position.x <= MidX)
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
#undef MinSize

