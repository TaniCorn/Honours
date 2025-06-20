#include "VoxelModelManager.h"
#include <DirectXMath.h>

VoxelModelManager::VoxelModelManager()
{
	Loader = std::make_unique<magicavoxel::VoxFile>(new magicavoxel::VoxFile(true, true));
}

VoxelModelManager::~VoxelModelManager()
{
}

bool VoxelModelManager::LoadModel(const std::string& IdentifierName, const std::string& FilePath)
{
		if (ModelIndex.find(IdentifierName) == ModelIndex.end())
		{
			try
			{
				Loader->Load(FilePath);
			}
			catch (const std::exception&)
			{
				return false;
			}

			// Loads in the most recent model
			ModelIndex[IdentifierName] = int(Loader->sparseModels().size()) - 1;
			// Palette is only stored at denseModel variant
			ColorPalette[IdentifierName] = Loader->denseModels().at(0).palette();
			return true;
		}

		return false;
}

std::unique_ptr<std::vector<Voxel>> VoxelModelManager::ConstructVoxelsFromModel(const std::string& IdentifierName) const
{
	// Try and load in a sparse model by name
	const magicavoxel::VoxSparseModel* SparseModel;
	try
	{
		 SparseModel = &Loader->sparseModels().at(ModelIndex.at(IdentifierName));
	}
	catch (const std::exception&)
	{
		return nullptr;
	}

	// Reserve the voxels on initialisation
	const size_t AmountOfVoxels = SparseModel->voxels().size();
	std::unique_ptr<std::vector<Voxel>> Voxels = std::make_unique<std::vector<Voxel>>(AmountOfVoxels);

	// Create a Voxel for each voxel in the Sparse Model, loading in color index and position. 
	// Translates data from loader to application
	for (size_t i = 0; i < AmountOfVoxels; i++)
	{
		uint8_t PosX = SparseModel->voxels()[i].x;
		uint8_t PosY = SparseModel->voxels()[i].z;
		uint8_t PosZ = SparseModel->voxels()[i].y;
		uint8_t Color = SparseModel->voxels()[i].color;

		Voxel Vox;
		Vox.color = Color;
		Vox.point.x = PosX;
		Vox.point.y = PosY;
		Vox.point.z = PosZ;
		Voxels.get()->push_back(Vox);
	}
	return Voxels;
}

magicavoxel::VoxSparseModel* VoxelModelManager::GetModel(const std::string& IdentifierName) const
{
	try
	{
		return &Loader->sparseModels().at(ModelIndex.at(IdentifierName));// List of non-empty voxels
	}
	catch (const std::exception&)
	{
		return nullptr;
	}
}

const int VoxelModelManager::GetModelDimensions(const std::string& IdentifierName) const
{
	const magicavoxel::VoxSparseModel* SparseModel;
	try
	{
		SparseModel = &Loader->sparseModels().at(ModelIndex.at(IdentifierName));
	}
	catch (const std::exception&)
	{
		return -1;
	}

	// Calculate the dimensions of the model based on the highest dimension
	int XRes = SparseModel->size().x;
	int YRes = SparseModel->size().y;
	int ZRes = SparseModel->size().z;
	int Res = max(XRes, YRes);
	Res = max(Res, ZRes);

	//TODO: There has to be a quicker way of doing this. 
	
	// Returns the next number that is 2^x and is bigger than the resolution of the model
	int Factorial = 0;
	for (int i = 0; i < 10; i++)
	{
		int Factorial = int(pow(2, i));
		if (Factorial > Res)
		{
			return Factorial;
		}
	}
	return -1;
}

const DirectX::XMFLOAT3 VoxelModelManager::GetModelDimensionsExact(const std::string& IdentifierName) const
{
	const magicavoxel::VoxSparseModel* SparseModel;
	try
	{
		SparseModel = &Loader->sparseModels().at(ModelIndex.at(IdentifierName));
	}
	catch (const std::exception&)
	{
		return DirectX::XMFLOAT3(-1,-1,-1);
	}

	float XRes = float(SparseModel->size().x);
	float YRes = float(SparseModel->size().y);
	float ZRes = float(SparseModel->size().z);
	return XMFLOAT3(XRes, YRes, ZRes);
}

const magicavoxel::Color VoxelModelManager::GetColorFromPalette(const std::string& IdentifierName, const uint8_t ColorIndex) const
{
	try
	{
		return ColorPalette.at(IdentifierName).at(ColorIndex);
	}
	catch (const std::exception&)
	{
		return magicavoxel::Color();
	}
}

const magicavoxel::Palette* VoxelModelManager::GetPalette(const std::string& IdentifierName) const
{
	try
	{
		return &ColorPalette.at(IdentifierName);
	}
	catch (const std::exception&)
	{
		return nullptr;
	}
}

DirectX::XMFLOAT4 VoxelModelManager::GetRGBAFromColor(const magicavoxel::Color color)
{
	return XMFLOAT4(color.r / 255.f, color.g / 255.f, color.b / 255.f, color.a / 255.f);
}
