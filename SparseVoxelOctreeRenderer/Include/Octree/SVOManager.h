#pragma once

#include "SVOGPURepresentation.h"
#include <vector>
class SVOManager
{
public:
	SVOManager();
	~SVOManager();

	void InitialiseSVOModel(const std::string& name, const int AmountOfVoxels, const int Resolution, const UINT VoxelSize = 1);
	bool CreateSVOModel(const std::string& name, const std::vector<Voxel>& Voxels);
	OctreeGPU::SVOGPURepresentation* GetSVOModel(const std::string& name)
	{
		if (SVOModels.find(name) != SVOModels.end())
		{
			return SVOModels[name].get();
		}
		else
		{
			return nullptr;
		}
	};

private:
	std::map<std::string, std::unique_ptr<OctreeGPU::SVOGPURepresentation>> SVOModels;
};

