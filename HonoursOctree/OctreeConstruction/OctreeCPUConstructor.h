#ifndef GPUOCTREE_H
#define GPUOCTREE_H


#include "DXF.h"
#include <set>
#include "../Resources/VoxelOctree.h"
#define MAX_DEPTH 20

/// <summary>
/// Offset based octree implementation
/// </summary>
class OctreeGPURepresentation
{
public:
    OctreeGPURepresentation();
    ~OctreeGPURepresentation();

    XMFLOAT3 TopLeftFrontPosition;
    XMFLOAT3 BottomRightBackPosition;
    XMFLOAT3 voxelPosition;
    UINT32 colorIndex;
    UINT32 depth;
    UINT32 octantsStride[8];

    XMFLOAT3 getChildTLFBound(const UINT32 octant) const;
    XMFLOAT3 getChildBRBBound(const UINT32 octant) const;
    int determineOctant(XMFLOAT3 position) const;
    bool shouldSubdivide() const;
private:
    bool inBoundary(const XMFLOAT3 position) const;

};

/// <summary>
/// Constructs an octree from scratch in an SRV format for usage in a compute shader.
/// </summary>
class GPUOctree {
public:
    GPUOctree(int amountOfVoxels) {
        octree = new OctreeGPURepresentation[amountOfVoxels*MAX_DEPTH];
    }
    ~GPUOctree() {
        delete[] octree;
        for (auto vox : voxels)
        {
            delete vox;
        }
        voxels.clear();
    };
    void InsertVoxel(Voxel* Vox);

    UINT32 maxStride = 0;
    OctreeGPURepresentation* octree;
    std::vector<Voxel*> voxels;
    void Clear();

};

#undef MAX_DEPTH
#endif // !GPUOCTREE_H
