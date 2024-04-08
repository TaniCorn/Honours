#pragma once
#include "DXF.h"
#include <set>
#include "../Resources/VoxelOctree.h"

class Octree
{
public:
    Octree();
    Octree(XMFLOAT3 topLeft, XMFLOAT3 bottomRight);
    ~Octree();

    XMFLOAT3 topLeftFrontPoint;
    XMFLOAT3 bottomRightBackPoint;
    Voxel* storedVoxel;

    Octree* TLF;
    Octree* TRF;
    Octree* BLF;
    Octree* BRF;
    
    Octree* TLB;
    Octree* TRB;
    Octree* BLB;
    Octree* BRB;

    void insert(Voxel* point, const float sizeOfVoxels);

    int depth = 0;
    /// <summary>
    /// Recursively adds up all the child octrees in this octree
    /// </summary>
    int getOctantAmount(Octree* oc);
    /// <summary>
    /// Recursively checks all child octrees to find the max depth
    /// </summary>
    int getMaxDepth(Octree* oc, int depth);
private:
    Voxel* search(XMFLOAT3 point);
    bool inBoundary(XMFLOAT3 point);

    int bigger(int a, int b);
};

class CPPOctree {
public:
    CPPOctree() {
    }
    ~CPPOctree() {
        delete octree;
        for (auto vox : voxels)
        {
            delete vox;
        }
        voxels.clear();
    };
    int octantAmount;
    float minSize;
    Octree* octree;
    std::vector<Voxel*> voxels;
};