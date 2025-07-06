#pragma once
#include <map>
#include <vector>
#include "vox_file.h"
#include "Voxel.h"

//Replace with include
#include "DXF.h"


class VoxelModelManager
{
public:
	VoxelModelManager();
	~VoxelModelManager();

	bool LoadModel(const std::string& IdentifierName, const std::string& FilePath);

	/// <summary>
	/// Constructs a series of Voxels from a MagicaVoxel model and returns them
	/// </summary>
	std::vector<Voxel> ConstructVoxelsFromModel(const std::string& IdentifierName) const;
	magicavoxel::VoxSparseModel* GetModel(const std::string& IdentifierName) const;

	/// <summary>
	/// Gets the next highest 2^x model dimension, so that all dimensions are encapsulated
	/// </summary>
	const int GetModelDimensions(const std::string& IdentifierName) const;
	const DirectX::XMFLOAT3 GetModelDimensionsExact(const std::string& IdentifierName) const;

	magicavoxel::Color GetColorFromPalette(const std::string& IdentifierName, const uint8_t ColorIndex) const;
	magicavoxel::Palette GetPalette(const std::string& IdentifierName);

	static DirectX::XMFLOAT4 GetRGBAFromColor(const magicavoxel::Color color);

private:
	std::unique_ptr<magicavoxel::VoxFile> Loader;
	std::map<std::string, int> ModelIndex;
	std::map<std::string, magicavoxel::Palette> ColorPalette;
};

