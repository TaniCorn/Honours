#include "SVOManager.h"

SVOManager::SVOManager()
{
}

SVOManager::~SVOManager()
{
}

SVOModel::SVOModel()
{
	SVO = nullptr;
	ColorPalette = magicavoxel::Palette();
}

void SVOManager::InitialiseSVOModel(const std::string& Name, const int AmountOfVoxels, const int Resolution, const UINT VoxelSize)
{
	if(SVOModels.find(Name) != SVOModels.end())
	{
		// Attempt to clear the existing SVO model if it exists and remove it from the map to overwriting it
		SVOModels[Name]->SVO->Clear();
		SVOModels.erase(Name);
	}
	else
	{
		ModelNames.push_back(Name);
	}

	std::unique_ptr<SVOModel> ModelStruct = std::make_unique<SVOModel>();
	std::unique_ptr<OctreeGPU::SVOGPURepresentation> SVO = std::make_unique<OctreeGPU::SVOGPURepresentation>(AmountOfVoxels, Resolution, VoxelSize);
	ModelStruct->SVO = std::move(SVO);
	
	SVOModels[Name] = std::move(ModelStruct);
}

bool SVOManager::CreateSVOModel(const std::string& Name, const std::vector<Voxel>& Voxels, magicavoxel::Palette Palette)
{
	// If the SVO model does not exist, we cannot insert voxels into it
	if(SVOModels.find(Name) == SVOModels.end())
	{
		return false;
	}

	SVOModels.at(Name)->ColorPalette = Palette;
	OctreeGPU::SVOGPURepresentation* SVO = SVOModels[Name]->SVO.get();

	// Allow the SVO to construct itself from the given voxels
	for (int i = 0; i < Voxels.size(); i++)
	{
		SVO->InsertVoxel(Voxels[i]);
	}
}

void SVOManager::CreateAndFlushSVOModel(const std::vector<Voxel>& Voxels, const int Resolution, const UINT VoxelSize)
{
	const int AmountOfVoxels = Voxels.size();

	std::unique_ptr<SVOModel> ModelStruct = std::make_unique<SVOModel>();
	std::unique_ptr<OctreeGPU::SVOGPURepresentation> SVO = std::make_unique<OctreeGPU::SVOGPURepresentation>(AmountOfVoxels, Resolution, VoxelSize);
	for (int i = 0; i < Voxels.size(); i++)
	{
		SVO->InsertVoxel(Voxels[i]);
	}
}


