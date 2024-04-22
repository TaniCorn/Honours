#pragma once
// Application.h
#ifndef _VOXMODELAPP_H
#define _VOXMODELAPP_H

// Includes
#include "DXF.h"	// include dxframework
#include "Quad/QuadTreeShader.h"
#include "Quad/Octree.h"
#include "vox_file.h"

class VoxModelApp : public BaseApplication
{
public:

	VoxModelApp();
	~VoxModelApp();
	void init(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight, Input* in, bool VSYNC, bool FULL_SCREEN);

	bool frame();

protected:
	bool render();
	void gui();

	void RecursiveOctreeLoop(Octree* quad_tree, float depth);
	void RenderCubeTree(const float depth, Octree* childQuad);

private:
	float screenw, screenh;
	XMFLOAT3 res;
	int maxDepth;

	QuadTreeShader* quadTreeShader;
	CubeMesh* cube;

	Octree* octree;
	std::vector<OctVoxel*> octpoints;

	magicavoxel::VoxFile* VoxModelLoader;
	magicavoxel::Palette colPalette;
	void LoadVoxModel();
	void RenderVoxModel();
	long long g;
};

#endif