#pragma once

#include "SVOGPURepresentation.h"
#include "vox_file.h"
#include <vector>

struct SVOModel
{
	SVOModel();
	std::unique_ptr<OctreeGPU::SVOGPURepresentation> SVO = nullptr;
	magicavoxel::Palette ColorPalette;
};

class SVOManager
{
public:
	SVOManager();
	~SVOManager();

	void InitialiseSVOModel(const std::string& name, const int AmountOfVoxels, const int Resolution, const UINT VoxelSize = 1);
	bool CreateSVOModel(const std::string& name, const std::vector<Voxel>& Voxels, magicavoxel::Palette Palette);


	OctreeGPU::SVOGPURepresentation* GetSVOModel(const std::string& Name)
	{
		if (SVOModels.find(Name) != SVOModels.end())
		{
			OctreeGPU::SVOGPURepresentation* t = SVOModels.at(Name)->SVO.get();
			return t;
		}
		else
		{
			return nullptr;
		}
	};
	const magicavoxel::Palette& GetPalette(const std::string& Name) const
	{
		if (SVOModels.find(Name) != SVOModels.end())
		{
			return SVOModels.at(Name)->ColorPalette;
		}
		else
		{
			return magicavoxel::Palette();
		}
	}

	// TODO: possibly think about caching the palettes and models in a way that they are in a single buffer so that we can map them to the GPU more efficiently

private:
	std::map<std::string, std::unique_ptr<SVOModel>> SVOModels;
};

