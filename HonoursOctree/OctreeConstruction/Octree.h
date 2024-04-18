#pragma once
#include "DXF.h"
#include <set>
#include "../Resources/VoxelOctree.h"

/// <summary>
/// Naive pointer based octree implementation
/// </summary>
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

    int getOctantAmount(Octree* oc);
    int getMaxDepth(Octree* oc, int depth);
private:
    /// <summary>
    /// Searches for a voxel at point, and returns it if there is one there
    /// </summary>
    Voxel* search(XMFLOAT3 point);
    /// <summary>
    /// Checks if the point is within the TLF and BRB bounds
    /// </summary>
    bool inBoundary(XMFLOAT3 point);

    /// <summary>
    /// returns the bigger betweent the two numbers
    /// </summary>
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