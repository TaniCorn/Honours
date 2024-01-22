#pragma once

//Implementation inspired from https://www.geeksforgeeks.org/quad-tree/
#include "DXF.h"
struct QuadPoint 
{
public:
	QuadPoint() {
		point = XMFLOAT2(0, 0);
	}
	QuadPoint(float x, float y) {
		point = XMFLOAT2(x, y);
	}
	void SetPoint(float x, float y) 
	{
		point.x = x; point.y = y;
	}
	void SetPoint(XMFLOAT2 xyz) 
	{
		point = xyz;
	}
	XMFLOAT2 GetPoint() {
		return point;
	}
	float GetX() {
		return point.x;
	}
	float GetY() {
		return point.y;
	}
private:
	XMFLOAT2 point;

};


struct QuadVoxel 
{
	QuadPoint point;
	int identifier;
};
class QuadTree
{
public:
	QuadTree();
	QuadTree(QuadPoint topLeft, QuadPoint bottomRight);
	~QuadTree();

	QuadPoint topLeftPoint;
	QuadPoint bottomRightPoint;
	QuadVoxel* n;
	std::vector<QuadVoxel*> points;

	QuadTree* topLeftQuad;
	QuadTree* topRightQuad;
	QuadTree* bottomLeftQuad;
	QuadTree* bottomRightQuad;

	void insert(QuadVoxel* point);
	QuadVoxel* search(QuadPoint point);
	bool inBoundary(QuadPoint point);
	int depth = 0;
private:

};

