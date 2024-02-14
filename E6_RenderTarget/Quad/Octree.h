#pragma once
#include "DXF.h"
struct OctPoint 
{
public:
    OctPoint() {
        point = XMFLOAT3(0, 0, 0);
    }
    OctPoint(float x, float y, float z) {
        point = XMFLOAT3(x, y, z);
    }
    void SetPoint(float x, float y, float z) 
    {
        point.x = x; point.y = y; point.z = z;
    }
    void SetPoint(XMFLOAT3 xyz) 
    {
        point = xyz;
    }
    XMFLOAT3 GetPoint() {
        return point;
    }
    float GetX() {
        return point.x;
    }
    float GetY() {
        return point.y;
    }
    float GetZ() {
        return point.z;
    }
private:
    XMFLOAT3 point;

};


struct OctVoxel 
{
    OctPoint point;
    int identifier;
    uint8_t color;
};

#include <set>
class Octree
{
public:
    Octree();
    Octree(OctPoint topLeft, OctPoint bottomRight);
    ~Octree();

    OctPoint topLeftFrontPoint;
    OctPoint bottomRightBackPoint;
    OctVoxel* storedVoxel;
    static std::set<OctVoxel*> points;

    Octree* TLF;
    Octree* TRF;
    Octree* BLF;
    Octree* BRF;
    
    Octree* TLB;
    Octree* TRB;
    Octree* BLB;
    Octree* BRB;

    void insert(OctVoxel* point);
    OctVoxel* search(OctPoint point);
    bool inBoundary(OctPoint point);
    int depth = 0;
    static int minSize;
    static int OctantAmount;
private:
    void internalInsert(Octree* child, OctPoint point);
};