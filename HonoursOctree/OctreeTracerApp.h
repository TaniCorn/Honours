#pragma once
// Application.h
#ifndef _COMPUTEVOXAPP_H
#define _COMPUTEVOXAPP_H

// Includes
#include "DXF.h"	// include dxframework
#include "OctreeConstruction/Octree.h"
#include "OctreeConstruction/OctreeConstructorShader.h"
#include "OctreeTraversal/OctreeTracerShader.h"
#include "VoxelModelLoader.h"
#include "UtilShaders/TextureShader.h"


class OctreeTracerApp : public BaseApplication
{
public:
	OctreeTracerApp();
	~OctreeTracerApp();
	void init(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight, Input* in, bool VSYNC, bool FULL_SCREEN);

	bool frame();

protected:
	bool render();
	void gui();
private:
	CPPOctree* loadVoxModel(std::string identifierName, std::string filepath);
	void generateVoxelModel(HWND hwnd, std::string identifierName, std::string filepath);
	void computeTracer();

	float screenw, screenh;
	struct VoxelOctreeModels 
	{
		CPPOctree* cppOctree;
		OctreeConstructorShader* octreeConstructor;
		ID3D11ShaderResourceView* voxelModel;
		magicavoxel::Palette* pallette;
	};

	std::map<std::string, VoxelOctreeModels> voxelModels;
	ID3D11ShaderResourceView* voxelModelResources[8];
	magicavoxel::Palette* voxelModelPalettes[8];
	OctreeTracerShader* octreeTracer;

	TextureShader* textureShader;
	RenderTexture* rt;
	OrthoMesh* om;

	int voxelViewMode = 0;
	int voxelViewDepth = 0;
	string viewModeDisplay;
	bool heatMap = false;
	float minSize = 1.0f;

	int voxelModelView = -1;
	VoxelModelLoader modelLoader;
	int ind = 0;
};

#endif