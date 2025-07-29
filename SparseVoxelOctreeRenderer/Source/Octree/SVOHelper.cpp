#include "SVOHelper.h"

bool SVOHelper::LoadModelInFromFile(const std::string& ModelName, const std::string& FilePath, SVOManager& SVOModels, VoxelModelManager& RawVoxelModels)
{
	if (RawVoxelModels.LoadModel(ModelName, FilePath))
	{
		std::vector<Voxel> Voxels = RawVoxelModels.GetVoxelsFromModel(ModelName);
		int VoxelModelResolution = RawVoxelModels.GetModelDimensions(ModelName);
		const UINT VoxelSize = 1;

		SVOModels.InitialiseSVOModel(ModelName, Voxels.size(), VoxelModelResolution, VoxelSize);

		magicavoxel::Palette ColorPalette = RawVoxelModels.GetPalette(ModelName);
		SVOModels.CreateSVOModel(ModelName, Voxels, ColorPalette);

		return true;
	}
	else
	{
		throw std::runtime_error("Error loading model"); // Remove when the traverser becomes safe
		return false;
	}
}
