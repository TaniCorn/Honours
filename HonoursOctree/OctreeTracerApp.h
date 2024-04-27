#pragma once
// Application.h
#ifndef _COMPUTEVOXAPP_H
#define _COMPUTEVOXAPP_H

// Includes
#include "DXF.h"	// include dxframework
#include "OctreeConstruction/NaiveOctree.h"
#include "OctreeConstruction/OctreeConstructorShader.h"
#include "OctreeTraversal/OctreeTracerShader.h"
#include "Resources/VoxelModelLoader.h"
#include "Utility/TextureShader.h"
#include "Utility/TimeMeasure.h"
#include "OctreeConstruction/OctreeCPUConstructor.h"

#define GPURECONSTRUCT 1
#define TMMEASURE 0
#define DYNAMICSCENE 0
#define MODELAMOUNT 8
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
	void loadModels();
	/// <summary>
	/// Calculates the tracer algorithm
	/// </summary>
	void computeTracer();

	void gpuReconstruction(HWND hwnd);
	void cpuReconstruction();

	GPUOctree* constructInCPU(std::string identifierName, GPUOctree* oc = nullptr);
	OctreeConstructorShader* constructInGPU(HWND hwnd, std::string identifierName, OctreeConstructorShader* oc = nullptr);

	void specificGPUConstruction(HWND hwnd, std::string identifierName, OctreeConstructorShader* oc = nullptr);
	void specificCPUConstruction(std::string identifierName, GPUOctree* oc = nullptr);

	float screenw, screenh;
	struct VoxelOctreeModels 
	{
		//NaiveCPUOctree* cppOctree;
		GPUOctree* gpuOctree;
		OctreeConstructorShader* octreeConstructor;
		//ID3D11ShaderResourceView* voxelModel;
		magicavoxel::Palette* pallette;
	};
	std::string modelNames[MODELAMOUNT];
	std::map<std::string, VoxelOctreeModels> voxelModels;
	ID3D11ShaderResourceView* voxelModelResources[MODELAMOUNT];
	GPUOctree* gpuOctreeRepresentation[MODELAMOUNT];
	magicavoxel::Palette* voxelModelPalettes[MODELAMOUNT];

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


	float recontime = 0.1f;
	int construction = 0;
	TimeMeasure tMeasure;
	TimeMeasure memMeasure;
	char userinput[20];

	int modelToConstruct = 0;
	int iterationSelection = 0;
	int iterationsForHeat = 25;
	int distanceSelection = 0;
};

#endif