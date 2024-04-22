#pragma once
// Application.h
#ifndef _QUADTREEAPP_H
#define _QUADTREEAPP_H

// Includes
#include "DXF.h"	// include dxframework
#include "QuadTreeShader.h"
#include "QuadTree.h"
#include "Octree.h"
#include "../BoundingVolumeHierarchy.h"
#include "../vox_file.h"

/*
 * Hello
 */
class QuadTreeApp : public BaseApplication
{
public:

	QuadTreeApp();
	~QuadTreeApp();
	void init(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight, Input* in, bool VSYNC, bool FULL_SCREEN);

	bool frame();

protected:
	bool render();
	void texturepass();
	void octpass();
	void bvhpass();

	void gui();

	void RecursiveQuadTreeLoop(QuadTree* quad_tree, float depth);
	void RenderPlaneQuad(const float depth, QuadTree* childQuad);
	int GetMaxDepth(QuadTree* quad_tree, int depth);

	void RecursiveOctreeLoop(Octree* quad_tree, float depth);
	void RenderCubeTree(const float depth, Octree* childQuad);

	void RecursiveBVHLoop(BoundingVolumeHierarchy* bvh, float depth);
	void RenderPlaneQuad(const float depth, BoundingVolumeHierarchy* bvh);
private:
	float screenw, screenh;
	float resolution = 16;
	int maxDepth;

	QuadTreeShader* quadTreeShader;
	CubeMesh* cube;
	PlaneMesh* plane;
	QuadTree* quadTree;
	QuadPoint* topLeftPoint;
	QuadPoint* bottomRightPoint;
	QuadVoxel* point;
	std::vector<QuadVoxel*> points;
	
	Octree* octree;
	std::vector<OctVoxel*> octpoints;

	BoundingVolumeHierarchy* BVH;

	magicavoxel::VoxFile* VoxModelLoader;
	magicavoxel::Palette colPalette;
	void LoadVoxModel();
	void RenderVoxModel();
};

#endif