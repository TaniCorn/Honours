#pragma once
#include "vox_file.h"
#include <map>
#include "VoxelOctree.h"

//NOTE: Z is up in magicavoxel


class VoxelModelLoader
{
public:
	VoxelModelLoader();
	~VoxelModelLoader();
	void loadModel(std::string identifierName, std::string filepath);
	
	//Model
	std::vector<Voxel> getModelVoxels(std::string name);
	const magicavoxel::VoxSparseModel& getModel(std::string name);
	//Dimensions
	int getModelDimensions(std::string modelName);
	XMFLOAT3 getModelDimensionsExact(std::string modelName);
	//Color
	const magicavoxel::Color getColorFromPalette(std::string modelName, uint8_t colorIndex);
	magicavoxel::Palette& getPalette(std::string modelName);
	static XMFLOAT4 getRGBAFromColor(magicavoxel::Color color);
private:
	magicavoxel::VoxFile* mvoxModelLoader;
	
	std::map<std::string, int> mVoxModels;
	std::map<std::string, magicavoxel::Palette*> mvoxColPalette;
};

