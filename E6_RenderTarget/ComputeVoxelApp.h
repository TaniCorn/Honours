#pragma once
// Application.h
#ifndef _COMPUTEVOXAPP_H
#define _COMPUTEVOXAPP_H

// Includes
#include "DXF.h"	// include dxframework
#include "Quad/QuadTreeShader.h"
#include "Quad/Octree.h"
#include "vox_file.h"
#include "ComputeVoxelShader.h"
#include "VoxelShader.h"
#include "ComputeOctreeShader.h"
class ComputeVoxelApp : public BaseApplication
{
public:
	ComputeVoxelApp();
	~ComputeVoxelApp();
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

	CubeMesh* cube;

	Octree* octree;
	std::vector<OctVoxel*> octpoints;

	magicavoxel::VoxFile* VoxModelLoader;
	magicavoxel::Palette colPalette;
	void LoadVoxModel();
	void RenderVoxModel();
	void ComputeVoxelRender();

	bool bRenderOctree = false;
	bool bReload = false;


	VoxelShader* voxelShader;
	ComputeVoxelShader* computeVoxels;
	RenderTexture* voxelRT;
	OrthoMesh* voxelOM;

	ComputeOctreeShader* GPUOctree;
};

#endif