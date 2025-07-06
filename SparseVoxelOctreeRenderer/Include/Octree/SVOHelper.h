#pragma once

#include <string>
#include "SVOGPURepresentation.h"
#include "VoxelModelManager.h"
#include "SVOManager.h"
#include "vox_file.h"
static class SVOHelper
{
public:

	/// <summary>
	/// Load in a model from disk via the VoxelModelManager and create it with a paleette and SVO representation in the SVOManager
	/// </summary>
	/// <param name="ModelName"> Identifier for the model</param>
	static bool LoadModelInFromFile(const std::string& ModelName, const std::string& FilePath, SVOManager& SVOModels, VoxelModelManager& RawVoxelModels);

};

