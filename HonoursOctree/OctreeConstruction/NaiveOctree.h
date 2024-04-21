#pragma once
#include "DXF.h"
#include <set>
#include "../Resources/VoxelOctree.h"

/// <summary>
/// Naive pointer based octree implementation
/// </summary>
class NaiveOctree
{
public:
    NaiveOctree();
    NaiveOctree(XMFLOAT3 topLeft, XMFLOAT3 bottomRight);
    ~NaiveOctree();

    XMFLOAT3 topLeftFrontPoint;
    XMFLOAT3 bottomRightBackPoint;
    Voxel* storedVoxel;

    NaiveOctree* TLF;
    NaiveOctree* TRF;
    NaiveOctree* BLF;
    NaiveOctree* BRF;
    
    NaiveOctree* TLB;
    NaiveOctree* TRB;
    NaiveOctree* BLB;
    NaiveOctree* BRB;

    void insert(Voxel* point, const float sizeOfVoxels);

    int depth = 0;

    int getOctantAmount(NaiveOctree* oc);
    int getMaxDepth(NaiveOctree* oc, int depth);
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

class NaiveCPUOctree {
public:
    NaiveCPUOctree() {
    }
    ~NaiveCPUOctree() {
        delete octree;
        for (auto vox : voxels)
        {
            delete vox;
        }
        voxels.clear();
    };

    void clear();
    int octantAmount;
    float minSize;
    NaiveOctree* octree;
    std::vector<Voxel*> voxels;
};