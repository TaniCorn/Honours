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

	void CPURecursiveOctreeLoop(Octree* quad_tree, float depth);
	void CPURenderCubeTree(const float depth, Octree* childQuad);
	void CPURenderVoxModel();
	void CopyGPUOctreeToCPU(ComputeOctreeShader* GPU, Octree* CPU);

	void RecursiveWriteCSV(Octree* CPU, std::ofstream& file);
	void WriteCPUToCSV(Octree* CPU, std::string name);
	void WriteVoxelsToCSV(std::string name);
	static int IDCSV;
	bool outputFile = false;
private:
	void RecursiveAddOctree(ComputeOctreeShader::VoxelOctree* oc, UINT32 currentStride, Octree* currentOctree);
	float screenw, screenh;
	XMFLOAT3 res;
	int maxDepth;

	CubeMesh* cube;
	QuadTreeShader* quadTreeShader;

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

	int VoxelViewMode = 0;
	int VoxelViewDepth = 0;
	string ViewModeDisplay;
	bool heatMap = false;
};

#endif