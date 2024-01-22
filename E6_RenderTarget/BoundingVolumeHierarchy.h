#pragma once
#include "DXF.h"

struct BVHPoint {
    BVHPoint() {
        point = XMFLOAT3(0, 0, 0);
    }
    BVHPoint(float x, float y, float z) {
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
class BoundingVolumeHierarchy
{
public:
    BoundingVolumeHierarchy();
    BoundingVolumeHierarchy(BVHPoint br, BVHPoint tL);

    void BuildBVH();
    bool inBoundary(BVHPoint point);
    void ResizeBounds();
    void ReverseResizeRecursion();
    std::vector<BVHPoint> points;
//protected:
	BoundingVolumeHierarchy* left;
	BoundingVolumeHierarchy* right;
    //AABB
    BVHPoint bottomRight;
    BVHPoint topLeft;
};

