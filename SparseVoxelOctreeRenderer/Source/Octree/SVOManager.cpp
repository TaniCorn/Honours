#include "SVOManager.h"

SVOManager::SVOManager()
{
}

SVOManager::~SVOManager()
{
}

void SVOManager::InitialiseSVOModel(const std::string& name, const int AmountOfVoxels, const int Resolution, const UINT VoxelSize)
{
	if(SVOModels.find(name) != SVOModels.end())
	{
		// Attempt to clear the existing SVO model if it exists and remove it from the map
		SVOModels[name]->Clear();
		SVOModels.erase(name);
	}

	// Create a new SVOGPURepresentation with the given resolution and voxels
	std::unique_ptr<OctreeGPU::SVOGPURepresentation> SVO = std::make_unique<OctreeGPU::SVOGPURepresentation>(AmountOfVoxels, Resolution, VoxelSize);
	SVOModels[name] = std::move(SVO);
}

bool SVOManager::CreateSVOModel(const std::string& name, const std::vector<Voxel>& Voxels)
{
	// If the SVO model does not exist, we cannot insert voxels into it
	if(SVOModels.find(name) == SVOModels.end())
	{
		return false;
	}


	OctreeGPU::SVOGPURepresentation* SVO = SVOModels[name].get();

	// Allow the SVO to construct itself from teh given voxels
	for (int i = 0; i < Voxels.size(); i++)
	{
		SVO->InsertVoxel(Voxels[i]);
	}
}
